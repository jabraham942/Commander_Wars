var Constructor = function()
{
    this.getName = function()
    {
        return qsTr("Feet");
    };
    this.getMovementpoints = function(terrain)
    {
        switch (terrain.getID())
        {
            case "PLAINS":
            case "PLAINS_DESTROYED":
            case "PLAINS_PLASMA":
            case "BEACH":
            case "BRIDGE":
            case "DESTROYEDWELD":
            case "FOREST":
            case "RUIN":
            case "STREET":
            case "WASTELAND":
            case "AIRPORT":
            case "FACTORY":
            case "HARBOUR":
            case "HQ":
            case "MINE":
            case "PIPESTATION":
            case "RADAR":
            case "TOWER":
            case "TOWN":
            case "SILO":
            case "SILO_ROCKET":
            case "LABOR":
            case "TEMPORARY_AIRPORT":
            case "TEMPORARY_HARBOUR":
                return 1;
            case "MOUNTAIN":
            case "RIVER":
                return 2;
        }
        return -1;
    };
};
Constructor.prototype = MOVEMENTTABLE;
var MOVE_FEET = new Constructor();
