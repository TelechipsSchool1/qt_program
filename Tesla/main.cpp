#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <Controllers/system.h>
#include <Controllers/dataprovider.h>
#include <Controllers/camerareceiveimage.h>
#include <opencvimageprovider.h>
#include <videostreamer.h>
#include "Controllers/sharedmemory.h"

// 전역 변수 정의
SharedMemory *shared_memory = nullptr;

int main(int argc, char *argv[])
{
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QGuiApplication app(argc, argv);

    // 공유 메모리 초기화
    initialize_shared_memory(&shared_memory);

    // 시스템 관련 객체
    System m_systemHandler;
    DataProvider dataProvider;
    CameraReceiver cameraReceiver;

    // 비디오 스트리밍 관련 객체
    VideoStreamer videoStreamer;
    VideoStreamer leftVideoStreamer;
    VideoStreamer rightVideoStreamer;

    OpencvImageProvider* leftImageProvider = new OpencvImageProvider();
    OpencvImageProvider* rightImageProvider = new OpencvImageProvider();

    // QML 엔진
    QQmlApplicationEngine engine;

    // QML 컨텍스트에 객체들 등록
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("systemHandler", &m_systemHandler);
    context->setContextProperty("dataProvider", &dataProvider);
    context->setContextProperty("cameraReceiver", &cameraReceiver);
    context->setContextProperty("videoStreamer", &videoStreamer);
    context->setContextProperty("leftVideoStreamer", &leftVideoStreamer);
    context->setContextProperty("rightVideoStreamer", &rightVideoStreamer);
    context->setContextProperty("leftImageProvider", leftImageProvider);
    context->setContextProperty("rightImageProvider", rightImageProvider);

    // 이미지 제공자 등록
    engine.addImageProvider("leftLive", leftImageProvider);
    engine.addImageProvider("rightLive", rightImageProvider);

    // QML 파일 로드
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    engine.load(url);

    // 비디오 스트리밍 및 이미지 업데이트 연결
    QObject::connect(&leftVideoStreamer, &VideoStreamer::newImage, leftImageProvider, &OpencvImageProvider::updateImage);
    QObject::connect(&rightVideoStreamer, &VideoStreamer::newImage, rightImageProvider, &OpencvImageProvider::updateImage);

    // 앱 실행
    return app.exec();
}
