var Constructor = function()
{
    this.getTerrainGroup = function()
    {
        return 2;
    };
    this.loadBaseSprite = function(terrain)
    {
        __BASEPIPELINE.loadBase(terrain, "desert_pipeline")
    };
    this.getTerrainSprites = function()
    {
        return __BASEPIPELINE.getSprites("desert_pipeline")
    };
    this.loadBaseTerrain = function(terrain, currentTerrainID)
    {
        if (currentTerrainID === "PLAINS")
        {
            terrain.loadBaseTerrain("PLAINS");
        }
        else if (currentTerrainID === "SNOW")
        {
            terrain.loadBaseTerrain("SNOW");
        }
        else if (currentTerrainID === "WASTE")
        {
            terrain.loadBaseTerrain("WASTE");
        }
        else
        {
            terrain.loadBaseTerrain("DESERT");
        }
    };
};
Constructor.prototype = __BASEPIPELINE;
var DESERT_PIPELINE = new Constructor();
