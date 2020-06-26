var Constructor = function()
{
    this.init = function(co)
    {
        co.setPowerStars(8);
        co.setSuperpowerStars(7);
    };

    this.activatePower = function(co)
    {
        var invasion = ["ARTILLERY", "FLAK", "LIGHT_TANK", "FLAK", "LIGHT_TANK"];
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(GameEnums.PowerMode_Power);
        dialogAnimation.queueAnimation(powerNameAnimation);
        CO_YUKIO.spawnUnits(co, 0.4, invasion, powerNameAnimation);
    };

    this.activateSuperpower = function(co, powerMode)
    {
        var invsion = ["HEAVY_TANK", "FLAK", "LIGHT_TANK", "ARTILLERY", "LIGHT_TANK", "K_HELI", "K_HELI"];
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(powerMode);
        powerNameAnimation.queueAnimationBefore(dialogAnimation);

        CO_YUKIO.spawnUnits(co, 0.7, invsion, powerNameAnimation);
        CO_YUKIO.yukioDamage(co, 3, powerNameAnimation);
    };


    this.yukioDamage = function(co, value, powerNameAnimation)
    {
        var player = co.getOwner();
        var animations = [];
        var counter = 0;
        var playerCounter = map.getPlayerCount();
        for (var i2 = 0; i2 < playerCounter; i2++)
        {
            var enemyPlayer = map.getPlayer(i2);
            if ((enemyPlayer !== player) &&
                (player.checkAlliance(enemyPlayer) === GameEnums.Alliance_Enemy))
            {

                var units = enemyPlayer.getUnits();
                units.randomize();
                for (i = 0; i < units.size() / 2; i++)
                {
                    var unit = units.at(i);
                    var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
                    animation.writeDataInt32(unit.getX());
                    animation.writeDataInt32(unit.getY());
                    animation.writeDataInt32(value);
                    animation.setEndOfAnimationCall("ANIMATION", "postAnimationDamage");
                    if (animations.length < 5)
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                        powerNameAnimation.queueAnimation(animation);
                        animations.push(animation);
                    }
                    else
                    {
                        animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
                        animations[counter].queueAnimation(animation);
                        animations[counter] = animation;
                        counter++;
                        if (counter >= animations.length)
                        {
                            counter = 0;
                        }
                    }
                }
                units.remove();
            }
        }
    };

    this.spawnUnits = function(co, count, invasion, powerNameAnimation)
    {
        var buildings = co.getOwner().getBuildings();
        var animations = [];
        var counter = 0;
        buildings.randomize();
        for (var i = 0; i < buildings.size() * count; i++)
        {
            var building = buildings.at(i);
            if (building.getBuildingID() === "TOWN")
            {
                if (map.getTerrain(building.getX(), building.getY()).getUnit() === null)
                {
                    map.spawnUnit(building.getX(), building.getY(), invasion[i % invasion.length], co.getOwner());
                    var animation = GameAnimationFactory.createAnimation(building.getX(), building.getY());
                    if (animations.length < 5)
                    {
                        animation.addSprite("power8", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                        powerNameAnimation.queueAnimation(animation);
                        animations.push(animation);
                    }
                    else
                    {
                        animation.addSprite("power8", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
                        animations[counter].queueAnimation(animation);
                        animations[counter] = animation;
                        counter++;
                        if (counter >= animations.length)
                        {
                            counter = 0;
                        }
                    }
                }
            }
        }
        buildings.remove();
    };

    this.loadCOMusic = function(co)
    {
        // put the co music in here.
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Power:
                audio.addMusic("resources/music/cos/bh_power.mp3", 1091 , 49930);
                break;
            case GameEnums.PowerMode_Superpower:
                audio.addMusic("resources/music/cos/bh_superpower.mp3", 3161 , 37731);
                break;
            case GameEnums.PowerMode_Tagpower:
                audio.addMusic("resources/music/cos/bh_tagpower.mp3", 779 , 51141);
                break;
            default:
                audio.addMusic("resources/music/cos/yukio.mp3", 100, 58286);
                break;
        }
    };

    this.getCOUnitRange = function(co)
    {
        return 4;
    };
    this.getCOArmy = function()
    {
        return "DM";
    };

    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                return 10;
            case GameEnums.PowerMode_Power:
                return 10;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker))
                {
                    return 10;
                }
                break;
        }
        return 0;
    };
    this.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        if (co.inCORange(Qt.point(defPosX, defPosY), defender) ||
            co.getPowerMode() > GameEnums.PowerMode_Off)
        {
            return 10;
        }
        return 0;
    };

    this.getTrueDamage = function(co, damage, attacker, atkPosX, atkPosY, attackerBaseHp,
                                  defender, defPosX, defPosY, isDefender)
    {
        // reduce counter damage by a flat amount here
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                if (damage >= 10)
                {
                    return 15
                }
                break;
            case GameEnums.PowerMode_Power:
                if (damage >= 10)
                {
                    return 15
                }
                break;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker))
                {
                    if (damage >= 10)
                    {
                        return 15
                    }
                }
                break;
        }
        return 0;
    };

    this.getDamageReduction = function(co, damage, attacker, atkPosX, atkPosY, attackerBaseHp,
                                  defender, defPosX, defPosY, isDefender, luckMode)
    {
        // reduce counter damage by a flat amount here
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                return 15;
            case GameEnums.PowerMode_Power:
                return 15;
            default:
                if (co.inCORange(Qt.point(defPosX, defPosY), defender))
                {
                    return 10;
                }
                break;
        }
        return 0;
    };

    // CO - Intel
    this.getBio = function()
    {
        return qsTr("Darc founder of the Dark Matter Cooperation. He's the primary reason for invasions of Sturm and Von Bolt. He uses war as a way of making money and doesn't care if people die.");
    };
    this.getHits = function()
    {
        return qsTr("Astromomic gains");
    };
    this.getMiss = function()
    {
        return qsTr("Any loses");
    };
    this.getCODescription = function()
    {
        return qsTr("As the leader of darc matter his troops have special armory and armor piercing weapons.");
    };
    this.getLongCODescription = function()
    {
        return qsTr("\nGlobal Effect: \nNone.") +
               qsTr("\n\nCO Zone Effect: \nDamage against is troops are reduced. Troops deal true damage if the base damage is at least 10%.");
    };
    this.getPowerDescription = function()
    {
        return qsTr("A small army spawns and fights for Yukio.");
    };
    this.getPowerName = function()
    {
        return qsTr("Invasion");
    };
    this.getSuperPowerDescription = function()
    {
        return qsTr("An army spawns and fights for Yukio. In order to support the invasion a bombardment dealing 3 Hp to half of the enemy troops is launched.");
    };
    this.getSuperPowerName = function()
    {
        return qsTr("Ground-Air-Invasion");
    };
    this.getPowerSentences = function()
    {
        return [qsTr("I show you, what true power means."),
                qsTr("I'm the evil that destroys you."),
                qsTr("Your weapons are the rubbish of my factories."),
                qsTr("The victory is mine!  Alea iacta est!"),
                qsTr("My troops are behind you."),
                qsTr("I'll become the ruler of the world.")];
    };
    this.getVictorySentences = function()
    {
        return [qsTr("Investment...Profits...great continue."),
                qsTr("War is just another tool to make Profit."),
                qsTr("Give me all your money and your lifes.")];
    };
    this.getDefeatSentences = function()
    {
        return [qsTr("Even with this loss i made profit..."),
                qsTr("I lost but i got your money to build a new army.")];
    };
    this.getName = function()
    {
        return qsTr("Yukio");
    };
}

Constructor.prototype = CO;
var CO_YUKIO = new Constructor();
