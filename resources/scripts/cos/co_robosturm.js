var Constructor = function()
{
    this.init = function(co)
    {
        co.setPowerStars(4);
        co.setSuperpowerStars(5);
    };

    this.loadCOMusic = function(co)
    {
        // put the co music in here.
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Power:
                audio.addMusic("resources/music/cos/bh_power.mp3");
                break;
            case GameEnums.PowerMode_Superpower:
                audio.addMusic("resources/music/cos/bh_superpower.mp3");
                break;
            case GameEnums.PowerMode_Tagpower:
                audio.addMusic("resources/music/cos/bh_tagpower.mp3");
                break;
            default:
                audio.addMusic("resources/music/cos/robosturm.mp3")
                break;
        }
    };

    this.activatePower = function(co)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(GameEnums.PowerMode_Power);
        dialogAnimation.queueAnimation(powerNameAnimation);

        CO_ROBOSTURM.power(co, 0, powerNameAnimation);
        audio.clearPlayList();
        CO_ROBOSTURM.loadCOMusic(co);
        audio.playRandom();
    };

    this.activateSuperpower = function(co, powerMode)
    {
        var dialogAnimation = co.createPowerSentence();
        var powerNameAnimation = co.createPowerScreen(powerMode);
        dialogAnimation.queueAnimation(powerNameAnimation);

        CO_ROBOSTURM.power(co, 2, powerNameAnimation);
        audio.clearPlayList();
        CO_ROBOSTURM.loadCOMusic(co);
        audio.playRandom();
    };

    this.power = function(co, value, powerNameAnimation)
    {

        var player = co.getOwner();
        var units = player.getUnits();
        var animations = [];
        var counter = 0;
        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);
            var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
            if (animations.length < 5)
            {
                animation.addSprite("power2", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                powerNameAnimation.queueAnimation(animation);
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power2", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
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

        var playerCounter = map.getPlayerCount();
        for (var i2 = 0; i2 < playerCounter; i2++)
        {
            var enemyPlayer = map.getPlayer(i2);
            if ((enemyPlayer !== player) &&
                (player.checkAlliance(enemyPlayer) === GameEnums.Alliance_Enemy))
            {

                units = enemyPlayer.getUnits();
                units.randomize();
                for (i = 0; i < units.size(); i++)
                {
                    unit = units.at(i);
                    animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());

                    animation.writeDataInt32(unit.getX());
                    animation.writeDataInt32(unit.getY());
                    animation.writeDataInt32(value);
                    animation.setEndOfAnimationCall("CO_ROBOSTURM", "postAnimationDamage");

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

    this.postAnimationDamage = function(postAnimation)
    {
        postAnimation.seekBuffer();
        var x = postAnimation.readDataInt32();
        var y = postAnimation.readDataInt32();
        var damage = postAnimation.readDataInt32();
        if (map.onMap(x, y))
        {
            var unit = map.getTerrain(x, y).getUnit();
            if (unit !== null)
            {
                if (damage > 0)
                {
                    var hp = unit.getHpRounded();
                    if (hp <= damage)
                    {
                        // set hp to very very low
                        unit.setHp(0.001);
                    }
                    else
                    {
                        unit.setHp(hp - damage);
                    }
                }
                // reduce ammo
                if (unit.getMaxAmmo2() > 0)
                {
                    unit.reduceAmmo2(unit.getAmmo2() / 2);
                }
                if (unit.getMaxAmmo1() > 0)
                {
                    unit.reduceAmmo1(unit.getAmmo1() / 2);
                }
            }

        }
    };

    this.getCOUnitRange = function(co)
    {
        return 2;
    };
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                return 0;
            case GameEnums.PowerMode_Power:
                return 0;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker))
                {
                    return -10;
                }
                break;
        }
        return -20;
    };



    this.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
                return 80;
            case GameEnums.PowerMode_Power:
                return 60;
            default:
                if (co.inCORange(Qt.point(defPosX, defPosY), defender))
                {
                    return 20;
                }
                break;
        }
        return 0;
    };

    this.getCOArmy = function()
    {
        return "MA";
    };
    this.getMovementpointModifier = function(co, unit, posX, posY)
    {
        if (co.getPowerMode() === GameEnums.PowerMode_Power)
        {
            return 2;
        }
        else if (co.getPowerMode() === GameEnums.PowerMode_Superpower ||
                 co.getPowerMode() === GameEnums.PowerMode_Tagpower)
        {
            return 3;
        }
        return 1;
    };

    // CO - Intel
    this.getBio = function()
    {
        return qsTr("Black Hole was in need of new strong CO's so Lash invented a second Sturm. But he doesn't accept any orders and formed his own Army.");
    };
    this.getHits = function()
    {
        return qsTr("Robots");
    };
    this.getMiss = function()
    {
        return qsTr("Livings");
    };
    this.getCODescription = function()
    {
        return qsTr("His troops can move 1 point more, but they have weaker firepower but higher defense capabilities.");
    };
    this.getPowerDescription = function()
    {
        return qsTr("Enemy looses half of their ammo and his troops can move 1 point more and their defense rises.");
    };
    this.getPowerName = function()
    {
        return qsTr("Machinzied Storm");
    };
    this.getSuperPowerDescription = function()
    {
        return qsTr("Enemy looses half of their ammo and 2 HP and his troops can move 2 point more and their defense rises extremly.");
    };
    this.getSuperPowerName = function()
    {
        return qsTr("Machinzied Destruction");
    };
    this.getPowerSentences = function()
    {
        return [qsTr("System Runtime. Error! No more Enemies found!"),
                qsTr("...Sturm is coming...you have no chance..."),
                qsTr("Order analyzied! Destroy Enemies."),
                qsTr("Nobody can kill a Robot."),
                qsTr("This is a war of a new time!"),
                qsTr("Humans befare the power of machines come to your country.")];
    };
    this.getVictorySentences = function()
    {
        return [qsTr(".........."),
                qsTr("...Enemy destroyed Country conquered."),
                qsTr("Robots are stronger than human beeings.")];
    };
    this.getDefeatSentences = function()
    {
        return [qsTr("No Programm found for this Situation."),
                qsTr("In accurate Attack this situation is impossible.")];
    };
    this.getName = function()
    {
        return qsTr("Robo-Sturm");
    };
}

Constructor.prototype = CO;
var CO_ROBOSTURM = new Constructor();
