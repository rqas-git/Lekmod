









include("HBFeatureGenerator");
local create = FeatureGenerator.Create;
local defaults = {iJunglePercent = 45, iForestPercent = 24, fMarshPercent = 9};
function FeatureGenerator.Create(args)
    return create(args, defaults);
end
