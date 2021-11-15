var Constructor = function()
{
    this.getTerrainGroup = function()
    {
        return 3;
    };
    this.loadBaseSprite = function(terrain)
    {
        __BASEPIPELINE.loadBase(terrain, "snow_pipeline")
    };
    this.getTerrainSprites = function()
    {
        return __BASEPIPELINE.getSprites("snow_pipeline")
    };
    this.loadBaseTerrain = function(terrain, currentTerrainID)
    {
        if (currentTerrainID === "PLAINS")
        {
            terrain.loadBaseTerrain("PLAINS");
        }
        else if (currentTerrainID === "DESERT")
        {
            terrain.loadBaseTerrain("DESERT");
        }
        else if (currentTerrainID === "WASTE")
        {
            terrain.loadBaseTerrain("WASTE");
        }
        else
        {
            terrain.loadBaseTerrain("SNOW");
        }
    };
};
Constructor.prototype = __BASEPIPELINE;
var SNOW_PIPELINE = new Constructor();
