#ifndef REPLAYRECORDER_H
#define REPLAYRECORDER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QBuffer>

#include "game/gamerecording/iReplayReader.h"

class GameMap;
using spGameMap = std::shared_ptr<GameMap>;

class ReplayRecorder final : public QObject, public IReplayReader
{
    Q_OBJECT
    enum class Type
    {
        Action,
        Map,
    };

    struct HeaderInfo
    {
        HeaderInfo(Type type)
            : m_type(type)
        {
        }
        HeaderInfo(QDataStream & stream)
        {
            stream >> m_version;
            qint32 type;
            stream >> type;
            m_type = static_cast<Type>(type);
            stream >> m_nextSeekPos;
        }
        qint32 m_version{1};
        Type m_type{Type::Action};
        qint64 m_nextSeekPos{0};
    };
    static const qint32 VERSION = 2;
public:
    explicit ReplayRecorder(GameMap* pMap);
    virtual ~ReplayRecorder();
    /**
     * @brief startRecording
     */
    void startRecording(const QString & file = "");
    /**
     * @brief recordAction
     * @param pAction
     */
    void recordAction(const spGameAction & action);
    /**
     * @brief continueRecording
     * @param file
     */
    bool continueRecording(const QString & file);
    /**
     * @brief loadRecord
     * @param filename
     */
    virtual bool loadRecord(const QString & filename) override;
    /**
     * @brief nextAction
     */
    virtual spGameAction nextAction() override;
    /**
     * @brief getMods
     * @return
     */
     virtual QStringList getMods() override
    {
        return m_mods;
    }
    /**
     * @brief seekToStart
     */
    void seekToStart();
    /**
     * @brief getProgess
     * @return
     */
    virtual qint32 getProgess() override
    {
        return m_progress;
    }
    /**
     * @brief getRecordSize
     * @return
     */
    virtual qint32 getRecordSize() override
    {
        return m_count;
    }
    /**
     * @brief getDayFromPosition
     * @param count
     * @return
     */
    virtual qint32 getDayFromPosition(qint32 count) override;
    /**
     * @brief seekToDay
     * @param day
     * @return
     */
    virtual void seekToDay(qint32 day) override;
    qint32 getCount() const;

    QString getRecordJson() const;

    static bool readRecordInfo(QDataStream & stream, QByteArray & jsonRecordInfo);
    static bool readRecordInfo(QDataStream & stream, QJsonObject & jsonRecordInfo);

private:
    bool validRecord(QByteArray & envData);
    void writeAction(const spGameAction & action);
    void writeMapState();
    ReplayRecorder::HeaderInfo seekToNextType(Type type, bool & success);
    QByteArray createRecordJson() const;
private:
    QFile m_recordFile;
    QDataStream m_stream{&m_recordFile};
    QStringList m_mods;
    QByteArray m_recordJson;
    qint32 m_count = 0;
    qint32 m_progress = 0;
    qint64 m_countPos = 0;
    qint64 m_mapPos = 0;
    qint64 m_streamStart = 0;

    bool m_recording {false};
    bool m_playing{false};
    qint32 m_currentDay{0};
    GameMap* m_pMap{nullptr};
};

#endif // REPLAYRECORDER_H
