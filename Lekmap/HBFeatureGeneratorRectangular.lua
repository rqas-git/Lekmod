------------------------------------------------------------------------------
--	FILE:               FeatureGenerator.lua
--	MODIFIED FOR CIV5:  Bob Thomas
--	PYTHON TO LUA:      Shaun Seckman
--	PURPOSE:            Default method for feature generation
------------------------------------------------------------------------------
--	Copyright (c) 2009, 2010 Firaxis Games, Inc. All rights reserved.
------------------------------------------------------------------------------

-- Rectangular maps share feature placement, with their original foliage defaults.
include("HBFeatureGenerator");
local create = FeatureGenerator.Create;
local defaults = {iJunglePercent = 45, iForestPercent = 24, fMarshPercent = 9};
function FeatureGenerator.Create(args)
    return create(args, defaults);
end
