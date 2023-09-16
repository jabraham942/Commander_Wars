#ifndef INGAMEINFOBAR_H
#define INGAMEINFOBAR_H

#include <QObject>

#include "3rd_party/oxygine-framework/oxygine/actor/SlidingActor.h"
#include "3rd_party/oxygine-framework/oxygine/actor/Box9Sprite.h"

#include "objects/minimap.h"

class GameMap;
class GameMenue;
class Player;
class IngameInfoBar;
using spIngameInfoBar = std::shared_ptr<IngameInfoBar>;

class IngameInfoBar final : public QObject, public oxygine::Actor
{
    Q_OBJECT
public:
    static constexpr qint32 spriteWidth = 127;
    static constexpr qint32 spriteHeigth = 192;

    explicit IngameInfoBar(GameMenue* pMenu, GameMap* pMap);
    ~IngameInfoBar() = default;
    Minimap* getMinimap()
    {
        return m_pMinimap.get();
    }
    void updateTerrainInfo(qint32 x, qint32 y, bool update);
    oxygine::spBox9Sprite getDetailedViewBox() const;
    void setMap(GameMap *newMap);
    Q_INVOKABLE GameMap *getMap() const;
    Q_INVOKABLE void updateMinimap();
    Q_INVOKABLE void updatePlayerInfo();
public slots:
    void updateCursorInfo(qint32 x, qint32 y);
    void syncMinimapPosition();
private:
    void addColorbar(float divider, qint32 posX, qint32 posY, QColor color);
    void createTerrainInfo(qint32 x, qint32 y);
    void createMovementInfo(qint32 x, qint32 y);
    bool createUnitInfo(qint32 x, qint32 y);
    void updateDetailedView(qint32 x, qint32 y);
    void updateInfoForPlayer(Player* pPlayer, qint32 & y, qint32 x1, qint32 x2, qint32 yAdvance, qint32 width);

private:
    spMinimap m_pMinimap;
    oxygine::spSlidingActor m_pMinimapSlider;
    oxygine::spBox9Sprite m_pGameInfoBox;
    oxygine::spBox9Sprite m_pCursorInfoBox;
    oxygine::spBox9Sprite m_pDetailedViewBox;
    qint32 m_LastX{-1};
    qint32 m_LastY{-1};
    GameMap* m_pMap{nullptr};
    GameMenue* m_pMenu{nullptr};
    qint32 m_shownPlayerCount{1};
};

#endif // INGAMEINFOBAR_H
