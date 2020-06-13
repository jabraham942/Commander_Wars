var Constructor = function()
{
    // called for loading the main sprite
    this.loadSprites = function(building, neutral)
    {
        if (building.getOwnerID() >= 0 && !neutral)
        {
            // none neutral player
            building.loadSprite("airport", false);
            building.loadSpriteV2("airport+mask", GameEnums.Recoloring_Table);
        }
        else
        {
            // neutral player
            building.loadSprite("airport+neutral", false);
        }
    };

    this.addCaptureAnimationBuilding = function(animation, building, startPlayer, capturedPlayer)
    {
        animation.addBuildingSprite("airport+mask", startPlayer , capturedPlayer, GameEnums.Recoloring_Table);
        animation.addBuildingSprite("airport", startPlayer , capturedPlayer, GameEnums.Recoloring_None);
    };
    this.getName = function()
    {
        return qsTr("Airport");
    };
    this.getActions = function()
    {
        // returns a string id list of the actions this building can perform
        return "ACTION_BUILD_UNITS";
    };

    this.getConstructionList = function(building)
    {
        return ["T_HELI", "K_HELI", "DUSTER", "TRANSPORTPLANE", "BOMBER", "FIGHTER", "STEALTHBOMBER", "BLACK_BOMB"];
    };

    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "fore_airport";
    };

    this.getTerrainAnimationBackground = function(unit, terrain)
    {
        return "back_airport";
    };

    this.getDescription = function()
    {
        return "<r>" + qsTr("Once captured can be used for ") + "</r>" +
               "<div c='#00ff00'>" + qsTr("production and resupplying") + "</div>" +
               "<r>" + qsTr(" of ") + "</r>" +
               "<div c='#00ff00'>" + qsTr("air") + "</div>" +
               "<r>" + qsTr(" units.") + "</r>";
    };

    this.getVisionHide = function(building)
    {
        return true;
    };
}

Constructor.prototype = BUILDING;
var AIRPORT = new Constructor();
