#include "mapselectionmapsmenue.h"

#include "coreengine/mainapp.h"
#include "coreengine/console.h"

#include "resource_management/backgroundmanager.h"
#include "resource_management/fontmanager.h"
#include "resource_management/objectmanager.h"
#include "resource_management/buildingspritemanager.h"
#include "resource_management/cospritemanager.h"

#include "resource_management/gamerulemanager.h"

#include "game/gamemap.h"

#include "game/player.h"

#include "game/co.h"

#include "menue/mainwindow.h"
#include "menue/gamemenue.h"

#include "objects/checkbox.h"
#include "objects/spinbox.h"


#include "QFileInfo"

MapSelectionMapsMenue::MapSelectionMapsMenue()
    : QObject()
{
    Mainapp* pApp = Mainapp::getInstance();
    this->moveToThread(pApp->getWorkerthread());

    Console::print("Entering Map Selection Menue", Console::eDEBUG);
    BackgroundManager* pBackgroundManager = BackgroundManager::getInstance();
    ObjectManager* pObjectManager = ObjectManager::getInstance();
    BuildingSpriteManager* pBuildingSpriteManager = BuildingSpriteManager::getInstance();
    pBuildingSpriteManager->loadAll();
    // load background
    oxygine::spSprite sprite = new oxygine::Sprite();
    addChild(sprite);
    oxygine::ResAnim* pBackground = pBackgroundManager->getResAnim("Background+1");
    sprite->setResAnim(pBackground);
    sprite->setPosition(0, 0);
    // background should be last to draw
    sprite->setPriority(static_cast<short>(Mainapp::ZOrder::Background));
    sprite->setScaleX(pApp->getSettings()->getWidth() / pBackground->getWidth());
    sprite->setScaleY(pApp->getSettings()->getHeight() / pBackground->getHeight());

    pApp->getAudioThread()->clearPlayList();
    pApp->getAudioThread()->loadFolder("resources/music/mapselection");
    pApp->getAudioThread()->playRandom();

    qint32 width = 0;
    if (pApp->getSettings()->getWidth() / 2 > 400)
    {
        width = 400;
    }
    else if (pApp->getSettings()->getWidth() / 2 < 300)
    {
        width = 300;
    }
    else
    {
        width = pApp->getSettings()->getWidth() / 2;
    }

    m_pMapSelection = new MapSelection(pApp->getSettings()->getHeight() - 70, width, "");
    m_pMapSelection->setPosition(10, 10);
    this->addChild(m_pMapSelection);
    m_pMinimap = new Minimap();
    m_pMinimap->setPosition(0, 0);
    m_pMinimap->setScale(2.0f);
    m_MinimapSlider = new oxygine::SlidingActor();

    m_MinimapSlider->setPosition(10, 10);
    m_MinimapSlider->setSize(pApp->getSettings()->getWidth() - width - 100 - 20,
                             pApp->getSettings()->getHeight() - 210 - 20);
    m_MinimapSlider->setContent(m_pMinimap);

    oxygine::ResAnim* pAnim = pObjectManager->getResAnim("panel");
    m_pMiniMapBox = new oxygine::Box9Sprite();
    m_pMiniMapBox->setResAnim(pAnim);
    m_pMiniMapBox->setPosition(width + 50, 50);
    m_pMiniMapBox->setSize(pApp->getSettings()->getWidth() - width - 100,
                           pApp->getSettings()->getHeight() - 210);
    m_pMiniMapBox->setVerticalMode(oxygine::Box9Sprite::STRETCHING);
    m_pMiniMapBox->setHorizontalMode(oxygine::Box9Sprite::STRETCHING);


    m_pMiniMapBox->addChild(m_MinimapSlider);
    addChild(m_pMiniMapBox);

    // building count
    pAnim = pObjectManager->getResAnim("mapSelectionBuildingInfo");
    m_pBuildingBackground = new oxygine::Box9Sprite();
    m_pBuildingBackground->setResAnim(pAnim);
    m_pBuildingBackground->setSize(pApp->getSettings()->getWidth() - width - 100, 60);
    m_pBuildingBackground->setPosition(m_pMiniMapBox->getX(),
                                     m_pMiniMapBox->getY() + m_pMiniMapBox->getHeight() + 20);
    m_pBuildingBackground->setVerticalMode(oxygine::Box9Sprite::STRETCHING);
    m_pBuildingBackground->setHorizontalMode(oxygine::Box9Sprite::STRETCHING);
    oxygine::TextStyle style = FontManager::getTimesFont10();
    style.color = oxygine::Color(255, 255, 255, 255);
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;
    style.multiline = false;

    oxygine::spSlidingActor slider = new oxygine::SlidingActor();
    slider->setSize(m_pBuildingBackground->getWidth() - 20, 100);
    oxygine::spActor content = new oxygine::Actor();
    content->setSize(pBuildingSpriteManager->getBuildingCount()* (GameMap::Imagesize + 12), 100);
    for (qint32 i = 0; i < pBuildingSpriteManager->getBuildingCount(); i++)
    {
        spBuilding building = new Building(pBuildingSpriteManager->getBuildingID(i));
        building->updateBuildingSprites();
        qint32 width = building->getBuildingWidth();
        qint32 heigth = building->getBuildingHeigth();
        building->setScaleX(1.0f / static_cast<float>(width));
        building->setScaleY(1.0f / static_cast<float>(heigth));
        building->setPosition(i * (GameMap::Imagesize + 12) + GameMap::Imagesize * (width - 1) / (width),
                              5 + GameMap::Imagesize / 2 + GameMap::Imagesize * (heigth - 1) / (heigth));
        content->addChild(building);
        oxygine::spTextField pText = new oxygine::TextField();
        pText->setText("0");
        pText->setPosition(2 + i * (GameMap::Imagesize + 12), 10 + GameMap::Imagesize * 1.2f);
        pText->setStyle(style);
        content->addChild(pText);
        m_BuildingCountTexts.push_back(pText);
    }
    slider->setX(10);
    slider->setContent(content);
    m_pBuildingBackground->addChild(slider);
    addChild(m_pBuildingBackground);

    connect(m_pMapSelection.get(), SIGNAL(itemChanged(QString)), this, SLOT(mapSelectionItemChanged(QString)), Qt::QueuedConnection);
    connect(m_pMapSelection.get(), SIGNAL(itemClicked(QString)), this, SLOT(mapSelectionItemClicked(QString)), Qt::QueuedConnection);

    oxygine::spButton pButtonBack = ObjectManager::createButton(tr("Back"));
    pButtonBack->setPosition(10, pApp->getSettings()->getHeight() - 10 - pButtonBack->getHeight());
    pButtonBack->attachTo(this);
    pButtonBack->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit buttonBack();
    });
    connect(this, SIGNAL(buttonBack()), this, SLOT(slotButtonBack()), Qt::QueuedConnection);

    m_pButtonNext = ObjectManager::createButton(tr("Next"));
    m_pButtonNext->setPosition(pApp->getSettings()->getWidth() - 10 - pButtonBack->getWidth(), pApp->getSettings()->getHeight() - 10 - pButtonBack->getHeight());
    m_pButtonNext->attachTo(this);
    m_pButtonNext->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit buttonNext();
    });
    connect(this, SIGNAL(buttonNext()), this, SLOT(slotButtonNext()), Qt::QueuedConnection);

    m_pButtonStart = ObjectManager::createButton(tr("Start Game"));
    m_pButtonStart->setPosition(pApp->getSettings()->getWidth() - 10 - pButtonBack->getWidth(), pApp->getSettings()->getHeight() - 10 - pButtonBack->getHeight());
    m_pButtonStart->attachTo(this);
    m_pButtonStart->addEventListener(oxygine::TouchEvent::CLICK, [=](oxygine::Event * )->void
    {
        emit buttonStartGame();
    });
    connect(this, SIGNAL(buttonStartGame()), this, SLOT(startGame()), Qt::QueuedConnection);

    qint32 yPos = 10;
    m_pPlayerSelection = new PlayerSelection(pApp->getSettings()->getWidth() - 20,
                                             pApp->getSettings()->getHeight() - yPos - 20 - pButtonBack->getHeight());
    m_pPlayerSelection->setPosition(10, yPos);
    addChild(m_pPlayerSelection);

    QSize size(pApp->getSettings()->getWidth() - 20,
               pApp->getSettings()->getHeight() - 40 * 2);
    m_pRuleSelection = new  Panel(true,  size, size);
    m_pRuleSelection->setPosition(10, 20);
    addChild(m_pRuleSelection);


    hidePlayerSelection();
    hideRuleSelection();
}

MapSelectionMapsMenue::~MapSelectionMapsMenue()
{

}

void MapSelectionMapsMenue::slotButtonBack()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    switch (m_MapSelectionStep)
    {
        case MapSelectionStep::selectMap:
        {
            Console::print("Leaving Map Selection Menue", Console::eDEBUG);
            oxygine::getStage()->addChild(new Mainwindow());
            oxygine::Actor::detach();
            break;
        }
        case MapSelectionStep::selectRules:
        {
            hideRuleSelection();
            showMapSelection();
            m_MapSelectionStep = MapSelectionStep::selectMap;
            break;
        }
        case MapSelectionStep::selectPlayer:
        {
            showRuleSelection();
            hidePlayerSelection();
            m_MapSelectionStep = MapSelectionStep::selectRules;
            break;
        }
    }
    pApp->continueThread();
}

void MapSelectionMapsMenue::slotButtonNext()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    switch (m_MapSelectionStep)
    {
        case MapSelectionStep::selectMap:
        {
            if (m_pCurrentMap != nullptr)
            {
                hideMapSelection();
                showRuleSelection();
                m_MapSelectionStep = MapSelectionStep::selectRules;
            }
            break;
        }
        case MapSelectionStep::selectRules:
        {
            hideRuleSelection();
            showPlayerSelection();
            m_MapSelectionStep = MapSelectionStep::selectPlayer;
            break;
        }
        case MapSelectionStep::selectPlayer:
        {
            break;
        }
    }
    pApp->continueThread();
}

void MapSelectionMapsMenue::mapSelectionItemClicked(QString item)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    QFileInfo info = m_pMapSelection->getCurrentFolder() + item;
    if (info.isFile())
    {
        emit buttonNext();
    }
    pApp->continueThread();
}

void MapSelectionMapsMenue::mapSelectionItemChanged(QString item)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    QFileInfo info = m_pMapSelection->getCurrentFolder() + item;
    if (info.isFile())
    {
        if (m_pCurrentMap != nullptr)
        {
            m_pCurrentMap->deleteMap();
            m_pCurrentMap = nullptr;
        }
        m_pCurrentMap = new GameMap(info.absoluteFilePath(), true);
        m_pMinimap->updateMinimap(m_pCurrentMap);
        m_MinimapSlider->setContent(m_pMinimap);
        m_MinimapSlider->snap();
        BuildingSpriteManager* pBuildingSpriteManager = BuildingSpriteManager::getInstance();
        for (qint32 i = 0; i < pBuildingSpriteManager->getBuildingCount(); i++)
        {
            qint32 count = m_pCurrentMap->getBuildingCount(pBuildingSpriteManager->getBuildingID(i));
            m_BuildingCountTexts[i]->setText(QString::number(count).toStdString().c_str());
        }
    }
    pApp->continueThread();
}

void MapSelectionMapsMenue::startWeatherChanged(qint32 value)
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pCurrentMap->getGameRules()->setStartWeather(value);
    pApp->continueThread();
}

void MapSelectionMapsMenue::weatherChancesChanged()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    for (qint32 i = 0; i < m_pCurrentMap->getGameRules()->getWeatherCount(); i++)
    {
        m_pCurrentMap->getGameRules()->changeWeatherChance(i, m_pWeatherSlider->getSliderValue(i));
    }
    pApp->continueThread();
}

void MapSelectionMapsMenue::hideMapSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pMapSelection->setVisible(false);
    m_pMiniMapBox->setVisible(false);
    m_pBuildingBackground->setVisible(false);
    pApp->continueThread();
}

void MapSelectionMapsMenue::showMapSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pMapSelection->setVisible(true);
    m_pMiniMapBox->setVisible(true);
    m_pBuildingBackground->setVisible(true);
    pApp->continueThread();
}

void MapSelectionMapsMenue::hideRuleSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pRuleSelection->setVisible(false);
    pApp->continueThread();
}

void MapSelectionMapsMenue::showRuleSelection()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    m_pRuleSelection->setVisible(true);
    GameRuleManager* pGameRuleManager = GameRuleManager::getInstance();
    if (m_pCurrentMap->getGameRules()->getWeatherCount() != pGameRuleManager->getWeatherCount())
    {
        qint32 weatherChance = 100 / pGameRuleManager->getWeatherCount();
        for (qint32 i = 0; i < pGameRuleManager->getWeatherCount(); i++)
        {
            m_pCurrentMap->getGameRules()->addWeather(pGameRuleManager->getWeatherID(i), weatherChance);
        }
    }
    m_pRuleSelection->clearContent();
    qint32 y = 20;
    // font style
    oxygine::TextStyle style = FontManager::getMainFont();
    style.color = oxygine::Color(255, 255, 255, 255);
    style.vAlign = oxygine::TextStyle::VALIGN_DEFAULT;
    style.hAlign = oxygine::TextStyle::HALIGN_LEFT;

    oxygine::spTextField textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Enviroment").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    y += 40;

    QVector<QString> weatherStrings;
    QVector<qint32> weatherChances;
    for (qint32 i = 0; i < m_pCurrentMap->getGameRules()->getWeatherCount(); i++)
    {
        Weather* pWeather = m_pCurrentMap->getGameRules()->getWeather(i);
        weatherStrings.append(pWeather->getWeatherName());
        weatherChances.append(m_pCurrentMap->getGameRules()->getWeatherChance(pWeather->getWeatherId()));
    }
    m_pWeatherSlider = new Multislider(weatherStrings, pApp->getSettings()->getWidth() - 80, weatherChances);
    m_pWeatherSlider->setPosition(30, y);
    m_pRuleSelection->addItem(m_pWeatherSlider);
    connect(m_pWeatherSlider.get(), &Multislider::signalSliderChanged, this, &MapSelectionMapsMenue::weatherChancesChanged, Qt::QueuedConnection);

    y += m_pWeatherSlider->getHeight();
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Random Weather: ").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    spCheckbox pCheckbox = new Checkbox();
    pCheckbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    m_pRuleSelection->addItem(pCheckbox);
    pCheckbox->setChecked(m_pCurrentMap->getGameRules()->getRandomWeather());
    connect(pCheckbox.get(), &Checkbox::checkChanged, m_pCurrentMap->getGameRules(), &GameRules::setRandomWeather, Qt::QueuedConnection);

    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Start Weather: ").toStdString().c_str());
    textField->setPosition(30, pCheckbox->getY() + 40);
    m_pRuleSelection->addItem(textField);

    spDropDownmenu startWeather = new DropDownmenu(200, weatherStrings);
    startWeather->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    startWeather->setCurrentItem(m_pCurrentMap->getGameRules()->getCurrentWeatherId());
    connect(startWeather.get(), &DropDownmenu::sigItemChanged, this, &MapSelectionMapsMenue::startWeatherChanged, Qt::QueuedConnection);
    m_pRuleSelection->addItem(startWeather);
    startWeatherChanged(0);

    y = textField->getY() + 50;

    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Gameplay").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    y += 40;
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Unit Ranking System: ").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    pCheckbox = new Checkbox();
    pCheckbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    m_pRuleSelection->addItem(pCheckbox);
    pCheckbox->setChecked(m_pCurrentMap->getGameRules()->getRankingSystem());
    connect(pCheckbox.get(), &Checkbox::checkChanged, m_pCurrentMap->getGameRules(), &GameRules::setRankingSystem, Qt::QueuedConnection);
    y += 40;
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("No CO Powers: ").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    pCheckbox = new Checkbox();
    pCheckbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    m_pRuleSelection->addItem(pCheckbox);
    pCheckbox->setChecked(m_pCurrentMap->getGameRules()->getNoPower());
    connect(pCheckbox.get(), &Checkbox::checkChanged, m_pCurrentMap->getGameRules(), &GameRules::setNoPower, Qt::QueuedConnection);
    y += 40;
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Fog Of War: ").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    QVector<QString> fogModes = {tr("Off"), tr("Fog of War")};
    spDropDownmenu fogOfWar = new DropDownmenu(200, fogModes);
    fogOfWar->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    fogOfWar->setCurrentItem(m_pCurrentMap->getGameRules()->getFogMode());
    connect(fogOfWar.get(), &DropDownmenu::sigItemChanged, m_pCurrentMap->getGameRules(), [=](qint32 value)
    {
        m_pCurrentMap->getGameRules()->setFogMode(static_cast<GameEnums::Fog>(value));
    });
    m_pRuleSelection->addItem(fogOfWar);
    y += 50;
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Unit Limit: ").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    spSpinBox pSpinbox = new SpinBox(150, 0, 9999);
    pSpinbox->setInfinityValue(0.0f);
    pSpinbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
    m_pRuleSelection->addItem(pSpinbox);
    pSpinbox->setCurrentValue(m_pCurrentMap->getGameRules()->getUnitLimit());
    connect(pSpinbox.get(), &SpinBox::sigValueChanged, m_pCurrentMap->getGameRules(), &GameRules::setUnitLimit, Qt::QueuedConnection);

    y += 50;
    textField = new oxygine::TextField();
    textField->setStyle(style);
    textField->setText(tr("Victory Rules").toStdString().c_str());
    textField->setPosition(30, y);
    m_pRuleSelection->addItem(textField);
    y += 40;
    for (qint32 i = 0; i < pGameRuleManager->getVictoryRuleCount(); i++)
    {
        QString ruleID = pGameRuleManager->getVictoryRuleID(i);
        spVictoryRule pRule = new VictoryRule(ruleID);
        QString inputType = pRule->getRuleType().toLower();
        if (inputType == "checkbox")
        {
            bool defaultValue = pRule->getDefaultValue();
            if (defaultValue)
            {
                m_pCurrentMap->getGameRules()->addVictoryRule(pRule);
            }
            // add a cool check box and a cool text
            QString labelName = pRule->getRuleName();
            textField = new oxygine::TextField();
            textField->setStyle(style);
            textField->setText(labelName.toStdString().c_str());
            textField->setPosition(30, i * 50 + y);
            m_pRuleSelection->addItem(textField);
            spCheckbox pCheckbox = new Checkbox();
            pCheckbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
            m_pRuleSelection->addItem(pCheckbox);
            pCheckbox->setChecked(defaultValue);
            connect(pCheckbox.get(), &Checkbox::checkChanged, [=](bool value)
            {
                if (value)
                {
                    m_pCurrentMap->getGameRules()->addVictoryRule(ruleID);
                }
                else
                {
                    m_pCurrentMap->getGameRules()->removeVictoryRule(ruleID);
                }
            });
        }
        else if (inputType == "spinbox")
        {
            qint32 defaultValue = pRule->getDefaultValue();
            qint32 startValue = pRule->getInfiniteValue();
            if (defaultValue != startValue)
            {
                m_pCurrentMap->getGameRules()->addVictoryRule(pRule);
            }
            QString labelName = pRule->getRuleName();
            textField = new oxygine::TextField();
            textField->setStyle(style);
            textField->setText(labelName.toStdString().c_str());
            textField->setPosition(30, i * 50 + y);
            m_pRuleSelection->addItem(textField);
            spSpinBox pSpinbox = new SpinBox(200, startValue, 9999);
            pSpinbox->setPosition(40 + textField->getTextRect().getWidth(), textField->getY());
            pSpinbox->setInfinityValue(startValue);
            m_pRuleSelection->addItem(pSpinbox);
            pSpinbox->setCurrentValue(defaultValue);
            connect(pSpinbox.get(), &SpinBox::sigValueChanged, [=](float value)
            {
                qint32 newValue = static_cast<qint32>(value);
                if (newValue == startValue)
                {
                    m_pCurrentMap->getGameRules()->removeVictoryRule(ruleID);
                }
                else
                {
                    spVictoryRule pRule = new VictoryRule(ruleID);
                    pRule->setRuleValue(newValue);
                    m_pCurrentMap->getGameRules()->addVictoryRule(pRule);
                }
            });
        }
    }
    m_pRuleSelection->setContentHeigth(90 + startWeather->getY() + pGameRuleManager->getVictoryRuleCount() * 50);
    pApp->continueThread();
}

void MapSelectionMapsMenue::showPlayerSelection()
{
    m_pButtonStart->setVisible(true);
    m_pButtonNext->setVisible(false);
    m_pPlayerSelection->setVisible(true);
    m_pPlayerSelection->showPlayerSelection();
}

void MapSelectionMapsMenue::hidePlayerSelection()
{
    m_pButtonStart->setVisible(false);
    m_pButtonNext->setVisible(true);
    m_pPlayerSelection->setVisible(false);
}


void MapSelectionMapsMenue::startGame()
{
    Mainapp* pApp = Mainapp::getInstance();
    pApp->suspendThread();
    // fix some stuff for the players based on our current input
    for (qint32 i = 0; i < m_pCurrentMap->getPlayerCount(); i++)
    {
        Player* pPlayer = GameMap::getInstance()->getPlayer(i);
        // resolve CO 1 beeing set and CO 0 not
        if ((pPlayer->getCO(0) == nullptr) &&
            (pPlayer->getCO(1) != nullptr))
        {
            pPlayer->swapCOs();
        }
        // resolve random CO
        if (pPlayer->getCO(0) != nullptr)
        {
            COSpriteManager* pCOSpriteManager = COSpriteManager::getInstance();
            while (pPlayer->getCO(0)->getCoID() == "CO_RANDOM")
            {
                pPlayer->setCO(pCOSpriteManager->getCOID(Mainapp::randInt(0, pCOSpriteManager->getCOCount() - 1)), 0);
            }
        }
        if (pPlayer->getCO(1) != nullptr)
        {
            COSpriteManager* pCOSpriteManager = COSpriteManager::getInstance();
            while ((pPlayer->getCO(1)->getCoID() == "CO_RANDOM") ||
                   (pPlayer->getCO(1)->getCoID() == pPlayer->getCO(0)->getCoID()))
            {
                pPlayer->setCO(pCOSpriteManager->getCOID(Mainapp::randInt(0, pCOSpriteManager->getCOCount() - 1)), 1);
            }
        }
        // define army of this player
        pPlayer->defineArmy();
    }

    GameMap* pMap = GameMap::getInstance();
    pMap->updateSprites();
    // start game
    Console::print("Leaving Map Selection Menue", Console::eDEBUG);
    oxygine::getStage()->addChild(new GameMenue());
    oxygine::Actor::detach();
    pApp->continueThread();
}
