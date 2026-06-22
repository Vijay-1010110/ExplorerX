#include "GeminiLLMClient.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QCoreApplication>

namespace ExplorerX::Platform {

GeminiLLMClient::GeminiLLMClient() {
}

QString GeminiLLMClient::GetApiKey() const {
    // Read from environment variable first
    QString key = qEnvironmentVariable("GEMINI_API_KEY");
    if (key.isEmpty()) {
        // Fallback for local development if needed, though secure storage is preferred.
        // Replace with dummy if absolutely necessary.
        key = "dummy-api-key"; 
    }
    return key;
}

std::future<Domain::Expected<Domain::AIIntent>> GeminiLLMClient::ParseIntent(const std::string& prompt) {
    return std::async(std::launch::async, [this, prompt]() -> Domain::Expected<Domain::AIIntent> {
        // Prepare the payload
        QString systemInstruction = "You are a filesystem intent parser. Respond ONLY in a valid JSON object matching the AIIntent struct format exactly. "
                                    "For example: {\"Action\": \"Search\", \"SearchQuery\": \"kind:pics last week\", \"TargetPaths\": []}. "
                                    "Actions can be: Copy, Move, Delete, Search, Unknown.";
        
        QJsonObject systemMsg;
        systemMsg["role"] = "user";
        QJsonArray partsSys;
        QJsonObject partSys;
        partSys["text"] = systemInstruction;
        partsSys.append(partSys);
        systemMsg["parts"] = partsSys;

        QJsonObject userMsg;
        userMsg["role"] = "user";
        QJsonArray partsUser;
        QJsonObject partUser;
        partUser["text"] = QString::fromStdString(prompt);
        partsUser.append(partUser);
        userMsg["parts"] = partsUser;

        QJsonArray contents;
        contents.append(systemMsg);
        contents.append(userMsg);

        QJsonObject root;
        root["contents"] = contents;
        
        // System instructions can also be natively passed in Gemini v1.5 API, 
        // but combining them via the prompt is robust across endpoint versions.

        QJsonDocument doc(root);
        QByteArray payload = doc.toJson(QJsonDocument::Compact);

        // Prepare Network Request
        QString endpointUrl = QString("https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=%1").arg(GetApiKey());
        QNetworkRequest request((QUrl(endpointUrl)));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Run Event Loop for this background thread
        QEventLoop loop;
        QNetworkAccessManager manager;
        QNetworkReply* reply = manager.post(request, payload);

        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            QString errStr = reply->errorString();
            reply->deleteLater();
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::NetworkUnavailable, errStr.toStdString(), 0});
        }

        QByteArray responseData = reply->readAll();
        reply->deleteLater();

        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Failed to parse JSON response", 0});
        }

        QJsonObject responseObj = responseDoc.object();
        QJsonArray candidates = responseObj["candidates"].toArray();
        if (candidates.isEmpty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty candidates from LLM", 0});
        }

        QJsonObject firstCandidate = candidates[0].toObject();
        QJsonObject content = firstCandidate["content"].toObject();
        QJsonArray parts = content["parts"].toArray();
        if (parts.isEmpty()) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "Empty parts from LLM", 0});
        }

        QString llmText = parts[0].toObject()["text"].toString();

        // LLM might wrap JSON in markdown blocks like ```json ... ```
        llmText = llmText.trimmed();
        if (llmText.startsWith("```json")) llmText.remove(0, 7);
        if (llmText.startsWith("```")) llmText.remove(0, 3);
        if (llmText.endsWith("```")) llmText.chop(3);
        llmText = llmText.trimmed();

        QJsonParseError intentParseError;
        QJsonDocument intentDoc = QJsonDocument::fromJson(llmText.toUtf8(), &intentParseError);
        
        if (intentParseError.error != QJsonParseError::NoError) {
            return Domain::MakeUnexpected(Domain::Error{Domain::ErrorCode::InvalidFormat, "LLM did not return valid JSON", 0});
        }

        QJsonObject intentObj = intentDoc.object();
        Domain::AIIntent intent;
        
        QString actionStr = intentObj["Action"].toString("Unknown");
        if (actionStr == "Copy") intent.Action = Domain::AIAction::Copy;
        else if (actionStr == "Move") intent.Action = Domain::AIAction::Move;
        else if (actionStr == "Delete") intent.Action = Domain::AIAction::Delete;
        else if (actionStr == "Search") intent.Action = Domain::AIAction::Search;
        else intent.Action = Domain::AIAction::Unknown;

        if (intentObj.contains("TargetPaths")) {
            QJsonArray pathsArr = intentObj["TargetPaths"].toArray();
            for (const auto& p : pathsArr) {
                intent.TargetPaths.push_back(Domain::Path(p.toString().toStdString()));
            }
        }

        intent.DestinationPath = Domain::Path(intentObj["DestinationPath"].toString().toStdString());
        intent.SearchQuery = intentObj["SearchQuery"].toString().toStdString();

        return intent;
    });
}

} // namespace ExplorerX::Platform
