CO_LIN.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                    defender, defPosX, defPosY, isDefender)
{
    if (co.getIsCO0() === true)
    {
        var seaAirUnits = CO_LIN.getSeaAirUnitIDS();
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (seaAirUnits.indexOf(attacker.getUnitID()) < 0)
            {
                return 50;
            }
            return 10;
        case GameEnums.PowerMode_Power:
            if (seaAirUnits.indexOf(attacker.getUnitID()) < 0)
            {
                return 30;
            }
            return 10;
        default:
            if (seaAirUnits.indexOf(attacker.getUnitID()) < 0)
            {
                return 10;
            }
            break;
        }
    }
    return 0;
};

CO_LIN.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                     defender, defPosX, defPosY, isDefender)
{
    if (co.getIsCO0() === true)
    {
        var seaAirUnits = CO_LIN.getSeaAirUnitIDS();
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (seaAirUnits.indexOf(defender.getUnitID()) < 0)
            {
                return 50;
            }
            return 10;
        case GameEnums.PowerMode_Power:
            if (seaAirUnits.indexOf(defender.getUnitID()) < 0)
            {
                return 30;
            }
            return 10;
        default:
            return 5;
        }
    }
    return 0;
};

CO_LIN.getVisionrangeModifier = function(co, unit, posX, posY)
{
    if (co.getIsCO0() === true)
    {
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            return 2;
        case GameEnums.PowerMode_Power:
            return 1;
        default:
            return 0;
        }
    }
    return 0;
};

CO_LIN.getPerfectVision = function(co)
{
    if (co.getIsCO0() === true)
    {
        // return true if forest and reafs etc are disabled in fog of war see sonja
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            return true;
        case GameEnums.PowerMode_Power:
            return true;
        default:
            return false;
        }
    }
};
