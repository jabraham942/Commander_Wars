var Constructor = function()
{
    this.getTerrainGroup = function()
    {
        return 4;
    };
    this.loadBaseSprite = function(terrain)
    {
        __BASEFOREST.loadBase(terrain, "WASTE_FOREST", "waste_forest+style0")
    };
    this.loadOverlaySprite = function(terrain)
    {
        __BASEFOREST.loadOverlay(terrain, "WASTE_FOREST", "waste_forest+style0");
    };

    this.getTerrainSprites = function()
    {
        return __BASEFOREST.getSprites("waste_forest+style0")
    };
    this.loadBaseTerrain = function(terrain, currentTerrainID)
    {
        if (currentTerrainID === "DESERT")
        {
            terrain.loadBaseTerrain("DESERT");
        }
        else if (currentTerrainID === "PLAINS")
        {
            terrain.loadBaseTerrain("PLAINS");
        }
        else if (currentTerrainID === "SNOW")
        {
            terrain.loadBaseTerrain("SNOW");
        }
        else
        {
            terrain.loadBaseTerrain("WASTE");
        }
    };
};
Constructor.prototype = __BASEFOREST;
var WASTE_FOREST = new Constructor();
