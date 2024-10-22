ALTER TABLE GoodyHuts ADD Production integer DEFAULT 0;
ALTER TABLE GoodyHuts ADD GoldenAge integer DEFAULT 0;
ALTER TABLE GoodyHuts ADD FreeTiles integer DEFAULT 0;
ALTER TABLE GoodyHuts ADD Science integer DEFAULT 0;
ALTER TABLE GoodyHuts ADD PantheonPercent integer DEFAULT 0;

-- Below 2 columns need NEW_GOODIES option to function
ALTER TABLE GoodyHuts ADD Food integer DEFAULT 0;
ALTER TABLE GoodyHuts ADD BorderGrowth integer DEFAULT 0;
