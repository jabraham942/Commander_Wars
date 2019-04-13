var Constructor = function()
{
    this.init = function(co)
    {
        co.setPowerStars(3);
        co.setSuperpowerStars(3);
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
            default:
                audio.addMusic("resources/music/cos/kindle.mp3")
                break;
        }
    };

    this.activatePower = function(co)
    {
        var units = co.getPlayer().getUnits();
        var animations = [];
        var counter = 0;

        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);
            unit.refill();

            var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
            if (animations.length < 5)
            {
                animation.addSprite("power5", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power5", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5);
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

        CO_KINDLE.kindleDamage(co, 3);

        audio.clearPlayList();
        CO_KINDLE.loadCOMusic(co);
        audio.playRandom();
    };

    this.kindleDamage = function(co, value)
    {
        var player = co.getPlayer();
        var units = player.getUnits();
        var animations = [];
        var counter = 0;
        var unit = null;
        var animation = null;
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
                    if (unit.getTerrain().getBuilding() !== null)
                    {
                        var hp = unit.getHpRounded();
                        if (hp <= value)
                        {
                            // set hp to very very low
                            unit.setHp(0.001);
                        }
                        else
                        {
                            unit.setHp(hp - value);
                        }


                        animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
                        if (animations.length < 5)
                        {
                            animation.addSprite("power4", -map.getImageSize() * 1.27, -map.getImageSize() * 1.27, 0, 1.5, globals.randInt(0, 400));
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
                }
                units.remove();
            }
        }
    };

    this.activateSuperpower = function(co)
    {
        var units = co.getPlayer().getUnits();
        var animations = [];
        var counter = 0;
        units.randomize();
        for (var i = 0; i < units.size(); i++)
        {
            var unit = units.at(i);
            unit.setHp(unit.getHpRounded() + 5);

            var animation = GameAnimationFactory.createAnimation(unit.getX(), unit.getY());
            if (animations.length < 5)
            {
                animation.addSprite("power12", -map.getImageSize() * 2, -map.getImageSize() * 2, 0, 1.5, globals.randInt(0, 400));
                animations.push(animation);
            }
            else
            {
                animation.addSprite("power12", -map.getImageSize() * 2, -map.getImageSize() * 2, 0, 1.5);
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

        audio.clearPlayList();
        CO_KINDLE.loadCOMusic(co);
        audio.playRandom();
    };

    this.getCOUnitRange = function(co)
    {
        return 2;
    };
    this.getCOArmy = function()
    {
        return "BG";
    };
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender)
    {
        var building = map.getTerrain(atkPosX, atkPosY).getBuilding();
        switch (co.getPowerMode())
        {
            case GameEnums.PowerMode_Superpower:
                var ret = 0;
                if (building !== null)
                {
                    ret = 130;
                }
                ret += co.getPlayer().getBuildingCount() * 3;
                return ret;
            case GameEnums.PowerMode_Power:
                if (building !== null)
                {
                    return 80;
                }
                break;
            default:
                if (co.inCORange(Qt.point(atkPosX, atkPosY)))
                {
                    if (building !== null)
                    {
                        return 40;
                    }
                }
                break;
        }
        if (building !== null)
        {
            return 20;
        }
        return 0;
    };
}

Constructor.prototype = CO;
var CO_KINDLE = new Constructor();
