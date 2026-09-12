













include("HBMapmakerUtilities");
include("NaturalWondersCustomMethods");










































AssignStartingPlots = {};










function AssignStartingPlots.Create()













	local iW, iH = Map.GetGridSize();
	local feature_atoll;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_ATOLL" then
			feature_atoll = thisFeature.ID;
		end
	end





	local findStarts = {


		__Init = AssignStartingPlots.__Init,
		__InitLuxuryWeights = AssignStartingPlots.__InitLuxuryWeights,
		__CustomInit = AssignStartingPlots.__CustomInit,
		ApplyHexAdjustment = AssignStartingPlots.ApplyHexAdjustment,
		GenerateRegions = AssignStartingPlots.GenerateRegions,
		ChooseLocations = AssignStartingPlots.ChooseLocations,
		BalanceAndAssign = AssignStartingPlots.BalanceAndAssign,
		PlaceNaturalWonders = AssignStartingPlots.PlaceNaturalWonders,
		PlaceResourcesAndCityStates = AssignStartingPlots.PlaceResourcesAndCityStates,
		

		MeasureStartPlacementFertilityOfPlot = AssignStartingPlots.MeasureStartPlacementFertilityOfPlot,
		MeasureStartPlacementFertilityInRectangle = AssignStartingPlots.MeasureStartPlacementFertilityInRectangle,
		MeasureStartPlacementFertilityOfLandmass = AssignStartingPlots.MeasureStartPlacementFertilityOfLandmass,
		RemoveDeadRows = AssignStartingPlots.RemoveDeadRows,
		DivideIntoRegions = AssignStartingPlots.DivideIntoRegions,
		ChopIntoThreeRegions = AssignStartingPlots.ChopIntoThreeRegions,
		ChopIntoTwoRegions = AssignStartingPlots.ChopIntoTwoRegions,
		CustomOverride = AssignStartingPlots.CustomOverride,


		MeasureTerrainInRegions = AssignStartingPlots.MeasureTerrainInRegions,
		DetermineRegionTypes = AssignStartingPlots.DetermineRegionTypes,
		PlaceImpactAndRipples = AssignStartingPlots.PlaceImpactAndRipples,
		MeasureSinglePlot = AssignStartingPlots.MeasureSinglePlot,
		EvaluateCandidatePlot = AssignStartingPlots.EvaluateCandidatePlot,
		IterateThroughCandidatePlotList = AssignStartingPlots.IterateThroughCandidatePlotList,
		FindStart = AssignStartingPlots.FindStart,
		FindCoastalStart = AssignStartingPlots.FindCoastalStart,
		FindStartWithoutRegardToAreaID = AssignStartingPlots.FindStartWithoutRegardToAreaID,
		

		AttemptToPlaceBonusResourceAtPlot = AssignStartingPlots.AttemptToPlaceBonusResourceAtPlot,
		AttemptToPlaceHillsAtPlot = AssignStartingPlots.AttemptToPlaceHillsAtPlot,
		AttemptToPlaceSmallStrategicAtPlot = AssignStartingPlots.AttemptToPlaceSmallStrategicAtPlot,
		FindFallbackForUnmatchedRegionPriority = AssignStartingPlots.FindFallbackForUnmatchedRegionPriority,
		AddStrategicBalanceResources = AssignStartingPlots.AddStrategicBalanceResources,
		AttemptToPlaceStoneAtGrassPlot = AssignStartingPlots.AttemptToPlaceStoneAtGrassPlot,
		NormalizeStartLocation = AssignStartingPlots.NormalizeStartLocation,
		NormalizeTeamLocations = AssignStartingPlots.NormalizeTeamLocations,
		

		ExaminePlotForNaturalWondersEligibility = AssignStartingPlots.ExaminePlotForNaturalWondersEligibility,
		ExamineCandidatePlotForNaturalWondersEligibility = AssignStartingPlots.ExamineCandidatePlotForNaturalWondersEligibility,
		CanBeThisNaturalWonderType = AssignStartingPlots.CanBeThisNaturalWonderType,
		GenerateLocalVersionsOfDataFromXML = AssignStartingPlots.GenerateLocalVersionsOfDataFromXML,
		GenerateNaturalWondersCandidatePlotLists = AssignStartingPlots.GenerateNaturalWondersCandidatePlotLists,
		AttemptToPlaceNaturalWonder = AssignStartingPlots.AttemptToPlaceNaturalWonder,


		AssignCityStatesToRegionsOrToUninhabited = AssignStartingPlots.AssignCityStatesToRegionsOrToUninhabited,
		CanPlaceCityStateAt = AssignStartingPlots.CanPlaceCityStateAt,
		ObtainNextSectionInRegion = AssignStartingPlots.ObtainNextSectionInRegion,
		PlaceCityState = AssignStartingPlots.PlaceCityState,
		PlaceCityStateInRegion = AssignStartingPlots.PlaceCityStateInRegion,
		PlaceCityStates = AssignStartingPlots.PlaceCityStates,
		NormalizeCityState = AssignStartingPlots.NormalizeCityState,
		NormalizeCityStateLocations = AssignStartingPlots.NormalizeCityStateLocations,


		PlaceResourceImpactCoastalMod = AssignStartingPlots.PlaceResourceImpactCoastalMod,
		ExpandCoastalRing = AssignStartingPlots.ExpandCoastalRing,

		GenerateGlobalResourcePlotLists = AssignStartingPlots.GenerateGlobalResourcePlotLists,
		PlaceResourceImpact = AssignStartingPlots.PlaceResourceImpact,
		ProcessResourceList = AssignStartingPlots.ProcessResourceList,
		PlaceSpecificNumberOfResources = AssignStartingPlots.PlaceSpecificNumberOfResources,
		IdentifyRegionsOfThisType = AssignStartingPlots.IdentifyRegionsOfThisType,
		SortRegionsByType = AssignStartingPlots.SortRegionsByType,
		AssignLuxuryToRegion = AssignStartingPlots.AssignLuxuryToRegion,
		GetLuxuriesSplitCap = AssignStartingPlots.GetLuxuriesSplitCap,
		GetCityStateLuxuriesTargetNumber = AssignStartingPlots.GetCityStateLuxuriesTargetNumber,
		GetDisabledLuxuriesTargetNumber = AssignStartingPlots.GetDisabledLuxuriesTargetNumber,
		AssignLuxuryRoles = AssignStartingPlots.AssignLuxuryRoles,
		GetListOfAllowableLuxuriesAtCitySite = AssignStartingPlots.GetListOfAllowableLuxuriesAtCitySite,
		GetRandomLuxuriesTargetNumber = AssignStartingPlots.GetRandomLuxuriesTargetNumber,
		GetListOfAllowableLuxuriesNearCitySite = AssignStartingPlots.GetListOfAllowableLuxuriesNearCitySite,
		GenerateLuxuryPlotListsAtCitySite = AssignStartingPlots.GenerateLuxuryPlotListsAtCitySite,
		GenerateLuxuryPlotListsNearCitySite = AssignStartingPlots.GenerateLuxuryPlotListsNearCitySite,
		GenerateLuxuryPlotListsInRegion = AssignStartingPlots.GenerateLuxuryPlotListsInRegion,
		GetIndicesForLuxuryType = AssignStartingPlots.GetIndicesForLuxuryType,
		GetRegionLuxuryTargetNumbers = AssignStartingPlots.GetRegionLuxuryTargetNumbers,
		GetWorldLuxuryTargetNumbers = AssignStartingPlots.GetWorldLuxuryTargetNumbers,
		PlaceMarble = AssignStartingPlots.PlaceMarble,
		PlaceLuxuries = AssignStartingPlots.PlaceLuxuries,
		PlaceSmallQuantitiesOfStrategics = AssignStartingPlots.PlaceSmallQuantitiesOfStrategics,
		PlaceFish = AssignStartingPlots.PlaceFish,
		PlaceFishMainland = AssignStartingPlots.PlaceFishMainland,
		PlaceSexyBonusAtCivStarts = AssignStartingPlots.PlaceSexyBonusAtCivStarts,
		AddExtraBonusesToHillsRegions = AssignStartingPlots.AddExtraBonusesToHillsRegions,
		AddModernMinorStrategicsToCityStates = AssignStartingPlots.AddModernMinorStrategicsToCityStates,
		PlaceOilInTheSea = AssignStartingPlots.PlaceOilInTheSea,
		FixResourceGraphics = AssignStartingPlots.FixResourceGraphics,
		PrintFinalResourceTotalsToLog = AssignStartingPlots.PrintFinalResourceTotalsToLog,
		GetMajorStrategicResourceQuantityValues = AssignStartingPlots.GetMajorStrategicResourceQuantityValues,
		GetSmallStrategicResourceQuantityValues = AssignStartingPlots.GetSmallStrategicResourceQuantityValues,
		PlaceStrategicAndBonusResources = AssignStartingPlots.PlaceStrategicAndBonusResources,
		

		startingPlots = {},
		method = 2,
		NoCoastInland = true,
		iNumCivs = 0,
		player_ID_list = {},
		plotDataIsCoastal = {},
		plotDataIsNextToCoast = {},
		plotDataIsThreeFromCoast = {},
		plotDataIsFourFromCoast = {},
		regionData = {},
		regionTerrainCounts = {},
		regionTypes = {},
		distanceData = table.fill(0, iW * iH),
		playerCollisionData = table.fill(false, iW * iH),
		playerCoastalCollisionData = table.fill(false, iW * iH),
		startLocationConditions = {},
		bModLuxes = false,


		bTeamGame,
		iNumTeamsOfCivs,
		teams_with_major_civs,
		number_civs_per_team,
		

		inhabited_WestX,
		inhabited_SouthY,
		inhabited_Width,
		inhabited_Height,


		naturalWondersData = table.fill(0, iW * iH),
		bWorldHasOceans,
		iBiggestLandmassID,
		iNumNW = 0,
		wonder_list = {},
		eligibility_lists = {},
		xml_row_numbers = {},
		placed_natural_wonder = {},
		feature_atoll,
		

		cityStatePlots = {},
		iNumCityStates = 0,
		iNumCityStatesUnassigned = 0,
		iNumCityStatesPerRegion = 0,
		iNumCityStatesUninhabited = 0,
		iNumCityStatesSharedLux = 0,
		iNumCityStatesLowFertility = 0,
		cityStateData = table.fill(0, iW * iH),
		city_state_region_assignments = table.fill(-1, 41),
		uninhabited_areas_coastal_plots = {},
		uninhabited_areas_inland_plots = {},
		iNumCityStatesDiscarded = 0,
		city_state_validity_table = table.fill(false, 41),
		

		resources = {},
		resource_setting,
		amounts_of_resources_placed = table.fill(0, 99),
		luxury_assignment_count = table.fill(0, 99),
		luxury_low_fert_compensation = table.fill(0, 99),
		region_low_fert_compensation = table.fill(0, 22);
		luxury_region_weights = {},
		luxury_fallback_weights = {},
		luxury_city_state_weights = {},
		strategicData = table.fill(0, iW * iH),
		luxuryData = table.fill(0, iW * iH),
		bonusData = table.fill(0, iW * iH),
		fishData = table.fill(0, iW * iH),
		seaOilData = table.fill(0, iW * iH),
		marbleData = table.fill(0, iW * iH),
		sheepData = table.fill(0, iW * iH),
		regions_sorted_by_type = {},
		region_luxury_assignment = {},
		iNumTypesUnassigned = 30,
		iNumMaxAllowedForRegions = 16,
		iNumTypesAssignedToRegions = 0,
		resourceIDs_assigned_to_regions = {},
		iNumTypesAssignedToCS = 3,
		resourceIDs_assigned_to_cs = {},
		iNumTypesSpecialCase = 0,
		resourceIDs_assigned_to_special_case = {},
		iNumTypesRandom = 0,
		resourceIDs_assigned_to_random = {},
		iNumTypesDisabled = 0,
		resourceIDs_not_being_used = {},
		totalLuxPlacedSoFar = 0,
		realtotalLuxPlacedSoFar = 0,








		global_luxury_plot_lists = {},
		coast_next_to_land_list = {},
		marsh_list = {},
		flood_plains_list = {},
		hills_open_list = {},
		hills_covered_list = {},
		hills_jungle_list = {},
		hills_forest_list = {},
		jungle_flat_list = {},
		forest_flat_list = {},
		desert_flat_no_feature = {},
		plains_flat_no_feature = {},
		dry_grass_flat_no_feature = {},
		fresh_water_grass_flat_no_feature = {},
		tundra_flat_including_forests = {},
		forest_flat_that_are_not_tundra = {},
		feature_atoll = feature_atoll,


		dry_plains_flat_no_feature = {},
		fresh_water_plains_flat_no_feature = {},
		tundra_flat_forest = {},
		desert_or_tundra_flat_no_feature = {},
		hills_open_no_tundra = {},
		hills_open_no_desert = {},
		hills_open_no_tundra_no_desert = {},
		hills_open_no_grass = {},
		hills_open_no_grass_no_tundra = {},
		hills_open_no_grass_no_plains = {},
		hills_covered_no_tundra = {},
		hills_covered_no_grass = {},
		hills_covered_no_grass_no_tundra = {},
		flat_covered = {},
		flat_covered_no_grass = {},
		flat_covered_no_tundra = {},
		flat_covered_no_grass_no_tundra = {},
		flat_open = {},
		flat_open_no_grass_no_plains = {},
		flat_open_no_tundra_no_desert = {},
		flat_open_no_desert = {},
		flat_desert_including_flood = {},










		grass_flat_no_feature = {},
		tundra_flat_no_feature = {},
		snow_flat_list = {},
		hills_list = {},
		land_list = {},
		coast_list = {},
		mainland_coast_list = {},
		mainland_coast_list_inner = {},
		mainland_coast_list_second = {},
		mainland_coast_list_outer = {},
		coast_list_panagaea = {},
		marble_list = {},
		extra_deer_list = {},
		desert_wheat_list = {},
		banana_list = {},
		barren_plots = 0,
		

		centerBias = 40,
		middleBias = 70,
		minFoodInner = 2,
		minProdInner = 1,
		minGoodInner = 3,
		minFoodMiddle = 4,
		minProdMiddle = 2,
		minGoodMiddle = 6,
		minFoodOuter = 4,
		minProdOuter = 4,
		minGoodOuter = 8,
		maxJunk = 5,



		firstRingYIsEven = {{0, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}},
		secondRingYIsEven = {
		{1, 2}, {1, 1}, {2, 0}, {1, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2}, {0, 2}
		},
		thirdRingYIsEven = {
		{1, 3}, {2, 2}, {2, 1}, {3, 0}, {2, -1}, {2, -2},
		{1, -3}, {0, -3}, {-1, -3}, {-2, -3}, {-2, -2}, {-3, -1},
		{-3, 0}, {-3, 1}, {-2, 2}, {-2, 3}, {-1, 3}, {0, 3}
		},
		firstRingYIsOdd = {{1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, 1}},
		secondRingYIsOdd = {		
		{1, 2}, {2, 1}, {2, 0}, {2, -1}, {1, -2}, {0, -2},
		{-1, -2}, {-1, -1}, {-2, 0}, {-1, 1}, {-1, 2}, {0, 2}
		},
		thirdRingYIsOdd = {		
		{2, 3}, {2, 2}, {3, 1}, {3, 0}, {3, -1}, {2, -2},
		{2, -3}, {1, -3}, {0, -3}, {-1, -3}, {-2, -2}, {-2, -1},
		{-3, 0}, {-2, 1}, {-2, 2}, {-1, 3}, {0, 3}, {1, 3}
		},

		direction_types = {
			DirectionTypes.DIRECTION_NORTHEAST,
			DirectionTypes.DIRECTION_EAST,
			DirectionTypes.DIRECTION_SOUTHEAST,
			DirectionTypes.DIRECTION_SOUTHWEST,
			DirectionTypes.DIRECTION_WEST,
			DirectionTypes.DIRECTION_NORTHWEST
			},
		

		wheat_ID, cow_ID, deer_ID, banana_ID, fish_ID, sheep_ID, stone_ID,
		iron_ID, horse_ID, coal_ID, oil_ID, aluminum_ID, uranium_ID,
		whale_ID, pearls_ID, ivory_ID, fur_ID, silk_ID,
		dye_ID, spices_ID, sugar_ID, cotton_ID, wine_ID, incense_ID,
		gold_ID, silver_ID, gems_ID, marble_ID,

		copper_ID, salt_ID, citrus_ID, truffles_ID, crab_ID, cocoa_ID,
		bison_ID,
		

		coffee_ID, tea_ID, tobacco_ID, amber_ID, jade_ID, olives_ID, perfume_ID, coral_ID, lapis_ID,


		lavender_ID, obsidian_ID, platinum_ID, poppy_ID, tin_ID,

		coconut_ID, hardwood_ID, lead_ID, maize_ID, pineapple_ID, potato_ID, rice_ID, rubber_ID, sulfur_ID, titanium_ID,
		

		EligibilityMethodNumber = {},
		OccurrenceFrequency = {},
		RequireBiggestLandmass = {},
		AvoidBiggestLandmass = {},
		RequireFreshWater = {},
		AvoidFreshWater = {},
		LandBased = {},
		RequireLandAdjacentToOcean = {},
		AvoidLandAdjacentToOcean = {},
		RequireLandOnePlotInland = {},
		AvoidLandOnePlotInland = {},
		RequireLandTwoOrMorePlotsInland = {},
		AvoidLandTwoOrMorePlotsInland = {},
		CoreTileCanBeAnyPlotType = {},
		CoreTileCanBeFlatland = {},
		CoreTileCanBeHills = {},
		CoreTileCanBeMountain = {},
		CoreTileCanBeOcean = {},
		CoreTileCanBeAnyTerrainType = {},
		CoreTileCanBeGrass = {},
		CoreTileCanBePlains = {},
		CoreTileCanBeDesert = {},
		CoreTileCanBeTundra = {},
		CoreTileCanBeSnow = {},
		CoreTileCanBeShallowWater = {},
		CoreTileCanBeDeepWater = {},
		CoreTileCanBeAnyFeatureType = {},
		CoreTileCanBeNoFeature = {},
		CoreTileCanBeForest = {},
		CoreTileCanBeJungle = {},
		CoreTileCanBeOasis = {},
		CoreTileCanBeFloodPlains = {},
		CoreTileCanBeMarsh = {},
		CoreTileCanBeIce = {},
		CoreTileCanBeAtoll = {},
		AdjacentTilesCareAboutPlotTypes = {},
		AdjacentTilesAvoidAnyland = {},
		AdjacentTilesRequireFlatland = {},
		RequiredNumberOfAdjacentFlatland = {},
		AdjacentTilesRequireHills = {},
		RequiredNumberOfAdjacentHills = {},
		AdjacentTilesRequireMountain = {},
		RequiredNumberOfAdjacentMountain = {},
		AdjacentTilesRequireHillsPlusMountains = {},
		RequiredNumberOfAdjacentHillsPlusMountains = {},
		AdjacentTilesRequireOcean = {},
		RequiredNumberOfAdjacentOcean = {},
		AdjacentTilesAvoidFlatland = {},
		MaximumAllowedAdjacentFlatland = {},
		AdjacentTilesAvoidHills = {},
		MaximumAllowedAdjacentHills = {},
		AdjacentTilesAvoidMountain = {},
		MaximumAllowedAdjacentMountain = {},
		AdjacentTilesAvoidHillsPlusMountains = {},
		MaximumAllowedAdjacentHillsPlusMountains = {},
		AdjacentTilesAvoidOcean = {},
		MaximumAllowedAdjacentOcean = {},
		AdjacentTilesCareAboutTerrainTypes = {},
		AdjacentTilesRequireGrass = {},
		RequiredNumberOfAdjacentGrass = {},
		AdjacentTilesRequirePlains = {},
		RequiredNumberOfAdjacentPlains = {},
		AdjacentTilesRequireDesert = {},
		RequiredNumberOfAdjacentDesert = {},
		AdjacentTilesRequireTundra = {},
		RequiredNumberOfAdjacentTundra = {},
		AdjacentTilesRequireSnow = {},
		RequiredNumberOfAdjacentSnow = {},
		AdjacentTilesRequireShallowWater = {},
		RequiredNumberOfAdjacentShallowWater = {},
		AdjacentTilesRequireDeepWater = {},
		RequiredNumberOfAdjacentDeepWater = {},
		AdjacentTilesAvoidGrass = {},
		MaximumAllowedAdjacentGrass = {},
		AdjacentTilesAvoidPlains = {},
		MaximumAllowedAdjacentPlains = {},
		AdjacentTilesAvoidDesert = {},
		MaximumAllowedAdjacentDesert = {},
		AdjacentTilesAvoidTundra = {},
		MaximumAllowedAdjacentTundra = {},
		AdjacentTilesAvoidSnow = {},
		MaximumAllowedAdjacentSnow = {},
		AdjacentTilesAvoidShallowWater = {},
		MaximumAllowedAdjacentShallowWater = {},
		AdjacentTilesAvoidDeepWater = {},
		MaximumAllowedAdjacentDeepWater = {},
		AdjacentTilesCareAboutFeatureTypes = {},
		AdjacentTilesRequireNoFeature = {},
		RequiredNumberOfAdjacentNoFeature = {},
		AdjacentTilesRequireForest = {},
		RequiredNumberOfAdjacentForest = {},
		AdjacentTilesRequireJungle = {},
		RequiredNumberOfAdjacentJungle = {},
		AdjacentTilesRequireOasis = {},
		RequiredNumberOfAdjacentOasis = {},
		AdjacentTilesRequireFloodPlains = {},
		RequiredNumberOfAdjacentFloodPlains = {},
		AdjacentTilesRequireMarsh = {},
		RequiredNumberOfAdjacentMarsh = {},
		AdjacentTilesRequireIce = {},
		RequiredNumberOfAdjacentIce = {},
		AdjacentTilesRequireAtoll = {},
		RequiredNumberOfAdjacentAtoll = {},
		AdjacentTilesAvoidNoFeature = {},
		MaximumAllowedAdjacentNoFeature = {},
		AdjacentTilesAvoidForest = {},
		MaximumAllowedAdjacentForest = {},
		AdjacentTilesAvoidJungle = {},
		MaximumAllowedAdjacentJungle = {},
		AdjacentTilesAvoidOasis = {},
		MaximumAllowedAdjacentOasis = {},
		AdjacentTilesAvoidFloodPlains = {},
		MaximumAllowedAdjacentFloodPlains = {},
		AdjacentTilesAvoidMarsh = {},
		MaximumAllowedAdjacentMarsh = {},
		AdjacentTilesAvoidIce = {},
		MaximumAllowedAdjacentIce = {},
		AdjacentTilesAvoidAtoll = {},
		MaximumAllowedAdjacentAtoll = {},
		TileChangesMethodNumber = {},
		ChangeCoreTileToMountain = {},
		ChangeCoreTileToFlatland = {},
		ChangeCoreTileTerrainToGrass = {},
		ChangeCoreTileTerrainToPlains = {},
		SetAdjacentTilesToShallowWater = {},
		
	}
	
	findStarts:__Init()
	
	findStarts:__InitLuxuryWeights()
	

	findStarts:__CustomInit()
	
	return findStarts
end

function AssignStartingPlots:__Init()

	self.plotDataIsCoastal, self.plotDataIsNextToCoast = GenerateNextToCoastalLandDataTables()
	self.plotDataIsThreeFromCoast = GenerateThreeFromCoastTable(self.plotDataIsCoastal, self.plotDataIsNextToCoast)



	local csvids = "";
	for resource_data in GameInfo.Resources() do
		table.insert(self.resources, resource_data);
		local resourceID = resource_data.ID;
		local resourceType = resource_data.Type;

		csvids = csvids .. resourceType .. "," .. resourceID .. "\n";
		if resourceType == "RESOURCE_WHEAT" then
			self.wheat_ID = resourceID;
		elseif resourceType == "RESOURCE_COW" then
			self.cow_ID = resourceID;
		elseif resourceType == "RESOURCE_DEER" then
			self.deer_ID = resourceID;
		elseif resourceType == "RESOURCE_BANANA" then
			self.banana_ID = resourceID;
		elseif resourceType == "RESOURCE_FISH" then
			self.fish_ID = resourceID;
		elseif resourceType == "RESOURCE_SHEEP" then
			self.sheep_ID = resourceID;
		elseif resourceType == "RESOURCE_STONE" then
			self.stone_ID = resourceID;

		elseif resourceType == "RESOURCE_IRON" then
			self.iron_ID = resourceID;
		elseif resourceType == "RESOURCE_HORSE" then
			self.horse_ID = resourceID;
		elseif resourceType == "RESOURCE_COAL" then
			self.coal_ID = resourceID;
		elseif resourceType == "RESOURCE_OIL" then
			self.oil_ID = resourceID;
		elseif resourceType == "RESOURCE_ALUMINUM" then
			self.aluminum_ID = resourceID;
		elseif resourceType == "RESOURCE_URANIUM" then
			self.uranium_ID = resourceID;

		elseif resourceType == "RESOURCE_WHALE" then
			self.whale_ID = resourceID;
		elseif resourceType == "RESOURCE_PEARLS" then
			self.pearls_ID = resourceID;
		elseif resourceType == "RESOURCE_IVORY" then
			self.ivory_ID = resourceID;
		elseif resourceType == "RESOURCE_FUR" then
			self.fur_ID = resourceID;
		elseif resourceType == "RESOURCE_SILK" then
			self.silk_ID = resourceID;
		elseif resourceType == "RESOURCE_DYE" then
			self.dye_ID = resourceID;
		elseif resourceType == "RESOURCE_SPICES" then
			self.spices_ID = resourceID;
		elseif resourceType == "RESOURCE_SUGAR" then
			self.sugar_ID = resourceID;
		elseif resourceType == "RESOURCE_COTTON" then
			self.cotton_ID = resourceID;
		elseif resourceType == "RESOURCE_WINE" then
			self.wine_ID = resourceID;
		elseif resourceType == "RESOURCE_INCENSE" then
			self.incense_ID = resourceID;
		elseif resourceType == "RESOURCE_GOLD" then
			self.gold_ID = resourceID;
		elseif resourceType == "RESOURCE_SILVER" then
			self.silver_ID = resourceID;
		elseif resourceType == "RESOURCE_GEMS" then
			self.gems_ID = resourceID;
		elseif resourceType == "RESOURCE_MARBLE" then
			self.marble_ID = resourceID;

		elseif resourceType == "RESOURCE_COPPER" then
			self.copper_ID = resourceID;
		elseif resourceType == "RESOURCE_SALT" then
			self.salt_ID = resourceID;
		elseif resourceType == "RESOURCE_CITRUS" then
			self.citrus_ID = resourceID;
		elseif resourceType == "RESOURCE_TRUFFLES" then
			self.truffles_ID = resourceID;
		elseif resourceType == "RESOURCE_CRAB" then
			self.crab_ID = resourceID;
		elseif resourceType == "RESOURCE_COCOA" then
			self.cocoa_ID = resourceID;
		elseif resourceType == "RESOURCE_BISON" then
			self.bison_ID = resourceID;

		elseif resourceType == "RESOURCE_COFFEE" then
			self.coffee_ID = resourceID;
		elseif resourceType == "RESOURCE_TEA" then
			self.tea_ID = resourceID;
		elseif resourceType == "RESOURCE_TOBACCO" then
			self.tobacco_ID = resourceID;
		elseif resourceType == "RESOURCE_AMBER" then
			self.amber_ID = resourceID;
		elseif resourceType == "RESOURCE_JADE" then
			self.jade_ID = resourceID;
		elseif resourceType == "RESOURCE_OLIVE" then
			self.olives_ID = resourceID;
		elseif resourceType == "RESOURCE_PERFUME" then
			self.perfume_ID = resourceID;
		elseif resourceType == "RESOURCE_CORAL" then
			self.coral_ID = resourceID;
		elseif resourceType == "RESOURCE_LAPIS" then
			self.lapis_ID = resourceID;

		elseif resourceType == "RESOURCE_LAVENDER" then
			self.lavender_ID = resourceID;
		elseif resourceType == "RESOURCE_OBSIDIAN" then
			self.obsidian_ID = resourceID;
		elseif resourceType == "RESOURCE_PLATINUM" then
			self.platinum_ID = resourceID;
		elseif resourceType == "RESOURCE_POPPY" then
			self.poppy_ID = resourceID;
		elseif resourceType == "RESOURCE_TIN" then
			self.tin_ID = resourceID;

		elseif resourceType == "RESOURCE_COCONUT" then
			self.coconut_ID = resourceID;
		elseif resourceType == "RESOURCE_HARDWOOD" then
			self.hardwood_ID = resourceID;
		elseif resourceType == "RESOURCE_LEAD" then
			self.lead_ID = resourceID;
		elseif resourceType == "RESOURCE_MAIZE" then
			self.maize_ID = resourceID;
		elseif resourceType == "RESOURCE_PINEAPPLE" then
			self.pineapple_ID = resourceID;
		elseif resourceType == "RESOURCE_POTATO" then
			self.potato_ID = resourceID;
		elseif resourceType == "RESOURCE_RICE" then
			self.rice_ID = resourceID;
		elseif resourceType == "RESOURCE_RUBBER" then
			self.rubber_ID = resourceID;
		elseif resourceType == "RESOURCE_SULFUR" then
			self.sulfur_ID = resourceID;
		elseif resourceType == "RESOURCE_TITANIUM" then
			self.titanium_ID = resourceID;
		end
	end

	if self.coral_ID ~= nil then
		self.bModLuxes = true;
	end

	print("########## Resource ID's ##########");
	print(csvids);
end

function AssignStartingPlots:__InitLuxuryWeights()



	


	
	if self.bModLuxes == true then

		self.luxury_region_weights[1] = {
		{self.fur_ID,		40},
		{self.marble_ID,	10},
		{self.silver_ID,	40},
		{self.amber_ID,		40},
		{self.salt_ID,		40},
		{self.gold_ID,		10},
		{self.copper_ID,	10},
		{self.gems_ID,		10},
		{self.jade_ID,		10},
		{self.lapis_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.obsidian_ID,	10},
		{self.coral_ID,		10},	};

		self.luxury_region_weights[2] = {
		{self.citrus_ID,	40},
		{self.cocoa_ID,		40},
		{self.spices_ID,	40},
		{self.sugar_ID,		40},
		{self.obsidian_ID,	40},
		{self.coconut_ID,	40},
		{self.rubber_ID,	40},
		{self.truffles_ID,	40},
		{self.silk_ID,		10},
		{self.dye_ID,		10},
		{self.fur_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};
		
		self.luxury_region_weights[3] = {
		{self.truffles_ID,	40},
		{self.marble_ID,	05},
		{self.silk_ID,		10},
		{self.dye_ID,		10},
		{self.fur_ID,		40},
		{self.coconut_ID,	10},
		{self.rubber_ID,	10},
		{self.citrus_ID,	10},
		{self.cocoa_ID,		10},
		{self.spices_ID,	10},
		{self.sugar_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};
		
		self.luxury_region_weights[4] = {
		{self.incense_ID,	40},
		{self.marble_ID,	05},
		{self.salt_ID,		40},
		{self.gold_ID,		40},
		{self.lapis_ID,		40},
		{self.obsidian_ID,	10},
		{self.copper_ID,	10},
		{self.silver_ID,	10},
		{self.amber_ID,		10},
		{self.gems_ID,		10},
		{self.jade_ID,		10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};
		
		self.luxury_region_weights[5] = {
		{self.gold_ID,		30},
		{self.marble_ID,	15},
		{self.silver_ID,	30},
		{self.copper_ID,	30},
		{self.gems_ID,		30},
		{self.salt_ID,		30},
		{self.jade_ID,		30},
		{self.amber_ID,		30},
		{self.lapis_ID,		30},
		{self.obsidian_ID,	30},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};

		
		self.luxury_region_weights[6] = {
		{self.incense_ID,	40},
		{self.marble_ID,	10},
		{self.ivory_ID,		40},
		{self.wine_ID,		40},
		{self.olives_ID,	40},
		{self.coffee_ID,	40},
		{self.tobacco_ID,	10},
		{self.tea_ID,		10},
		{self.perfume_ID,	40},
		{self.cotton_ID,	10},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};
		
		self.luxury_region_weights[7] = {
		{self.tobacco_ID,	40},
		{self.marble_ID,	10},
		{self.tea_ID,		40},
		{self.cotton_ID,	40},
		{self.perfume_ID,	25},
		{self.ivory_ID,		10},
		{self.wine_ID,		10},
		{self.olives_ID,	25},
		{self.coffee_ID,	25},
		{self.whale_ID,		10},
		{self.crab_ID,		10},
		{self.pearls_ID,	10},
		{self.coral_ID,		10},	};
		
		self.luxury_region_weights[8] = {
		{self.gold_ID,		30},
		{self.marble_ID,	15},
		{self.silver_ID,	30},
		{self.copper_ID,	30},
		{self.gems_ID,		30},
		{self.salt_ID,		30},
		{self.jade_ID,		30},
		{self.amber_ID,		30},
		{self.lapis_ID,		30},
		{self.obsidian_ID,	30},
		{self.coffee_ID,	05},
		{self.coconut_ID,	05},
		{self.rubber_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.ivory_ID,		05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.incense_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.fur_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.sugar_ID,		05},
		{self.whale_ID,		20},
		{self.crab_ID,		20},
		{self.pearls_ID,	20},
		{self.coral_ID,		20},	};

		self.luxury_region_weights[9] = {
		{self.tobacco_ID,	40},
		{self.tea_ID,		40},
		{self.perfume_ID,	20},
		{self.cotton_ID,	30},
		{self.olives_ID,	20},
		{self.silver_ID,	20},
		{self.sugar_ID,		20},
		{self.copper_ID,	20},
		{self.coral_ID,		20},
		{self.crab_ID,		25},
		{self.pearls_ID,	25},
		{self.coconut_ID,	30},
		{self.rubber_ID,	05},
		{self.whale_ID,		25},
		{self.cocoa_ID,		10},
		{self.truffles_ID,	05},
		{self.spices_ID,	05},
		{self.gems_ID,		20},	};
		
		self.luxury_fallback_weights = {
		{self.gold_ID,		10},
		{self.silver_ID,	10},
		{self.copper_ID,	10},
		{self.gems_ID,		10},
		{self.marble_ID,	05},
		{self.salt_ID,		10},
		{self.jade_ID,		10},
		{self.amber_ID,		10},
		{self.lapis_ID,		10},
		{self.obsidian_ID,	10},
		{self.coffee_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.ivory_ID,		05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.incense_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.fur_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.sugar_ID,		05},
		{self.whale_ID,		30},
		{self.crab_ID,		30},
		{self.pearls_ID,	30},
		{self.coconut_ID,	05},
		{self.rubber_ID,	05},
		{self.coral_ID,		30},	};

		self.luxury_city_state_weights = {
		{self.gold_ID,		05},
		{self.obsidian_ID,	05},
		{self.marble_ID,	05},
		{self.silver_ID,	05},
		{self.copper_ID,	05},
		{self.gems_ID,		05},
		{self.salt_ID,		05},
		{self.jade_ID,		05},
		{self.amber_ID,		05},
		{self.lapis_ID,		05},
		{self.coffee_ID,	05},
		{self.tobacco_ID,	05},
		{self.tea_ID,		05},
		{self.perfume_ID,	05},
		{self.cotton_ID,	05},
		{self.ivory_ID,		05},
		{self.wine_ID,		05},
		{self.olives_ID,	05},
		{self.incense_ID,	05},
		{self.truffles_ID,	05},
		{self.silk_ID,		05},
		{self.dye_ID,		05},
		{self.fur_ID,		05},
		{self.citrus_ID,	05},
		{self.cocoa_ID,		05},
		{self.spices_ID,	05},
		{self.sugar_ID,		05},
		{self.coconut_ID,	05},
		{self.rubber_ID,	05},
		{self.whale_ID,		30},
		{self.crab_ID,		30},
		{self.pearls_ID,	30},
		{self.coral_ID,		30},	};
	else
		self.luxury_region_weights[1] = {
		{self.fur_ID,		40},
		{self.marble_ID,	10},
		{self.whale_ID,		25},
		{self.crab_ID,		25},
		{self.pearls_ID,	25},
		{self.silver_ID,	25},
		{self.copper_ID,	15},
		{self.salt_ID,		20},
		{self.gems_ID,		05},
		{self.dye_ID,		05},	};

		self.luxury_region_weights[2] = {
		{self.cocoa_ID,		35},
		{self.citrus_ID,	35},
		{self.spices_ID,	35},
		{self.coconut_ID,	35},
		{self.rubber_ID,	35},
		{self.gems_ID,		25},
		{self.sugar_ID,		20},
		{self.pearls_ID,	25},
		{self.copper_ID,	05},
		{self.truffles_ID,	25},
		{self.crab_ID,		25},
		{self.whale_ID,		25},
		{self.silk_ID,		25},
		{self.dye_ID,		25},	};
		
		self.luxury_region_weights[3] = {
		{self.dye_ID,		10},
		{self.silk_ID,		10},
		{self.truffles_ID,	30},
		{self.coconut_ID,	30},
		{self.rubber_ID,	10},
		{self.fur_ID,		10},
		{self.spices_ID,	10},
		{self.citrus_ID,	05},
		{self.salt_ID,		05},
		{self.copper_ID,	05},
		{self.cocoa_ID,		05},
		{self.crab_ID,		25},
		{self.whale_ID,		25},
		{self.pearls_ID,	25},	};
		
		self.luxury_region_weights[4] = {
		{self.incense_ID,	35},
		{self.salt_ID,		25},
		{self.marble_ID,	05},
		{self.gold_ID,		25},
		{self.copper_ID,	25},
		{self.cotton_ID,	15},
		{self.sugar_ID,		15},
		{self.pearls_ID,	25},
		{self.crab_ID,		25},
		{self.whale_ID,		25},
		{self.citrus_ID,	05},	};
		
		self.luxury_region_weights[5] = {
		{self.gold_ID,		30},
		{self.marble_ID,	15},
		{self.silver_ID,	30},
		{self.copper_ID,	30},
		{self.gems_ID,		30},
		{self.pearls_ID,	25},
		{self.salt_ID,		20},
		{self.crab_ID,		25},
		{self.whale_ID,		25},	};
		
		self.luxury_region_weights[6] = {
		{self.ivory_ID,		35},
		{self.wine_ID,		35},
		{self.marble_ID,	05},
		{self.salt_ID,		05},
		{self.incense_ID,	25},
		{self.spices_ID,	25},
		{self.whale_ID,		25},
		{self.pearls_ID,	25},
		{self.crab_ID,		25},
		{self.truffles_ID,	25},
		{self.rubber_ID,	25},
		{self.gold_ID,		25},	};
		
		self.luxury_region_weights[7] = {
		{self.cotton_ID,	30},
		{self.marble_ID,	10},
		{self.silver_ID,	20},
		{self.sugar_ID,		20},
		{self.copper_ID,	20},
		{self.crab_ID,		25},
		{self.pearls_ID,	25},
		{self.whale_ID,		25},
		{self.cocoa_ID,		25},
		{self.truffles_ID,	05},
		{self.spices_ID,	05},
		{self.gems_ID,		25},	};
		
		self.luxury_region_weights[8] = {
		{self.ivory_ID,		15},
		{self.cotton_ID,	15},
		{self.wine_ID,		15},
		{self.marble_ID,	10},
		{self.silver_ID,	10},
		{self.salt_ID,		05},
		{self.copper_ID,	20},
		{self.whale_ID,		25},
		{self.pearls_ID,	25},
		{self.crab_ID,		25},
		{self.truffles_ID,	10},
		{self.cocoa_ID,		10},
		{self.spices_ID,	05},
		{self.sugar_ID,		05},
		{self.citrus_ID,	05},
		{self.incense_ID,	05},
		{self.silk_ID,		05},
		{self.coconut_ID,	10},
		{self.rubber_ID,	10},
		{self.gems_ID,		15},
		{self.gold_ID,		05},	};

		self.luxury_region_weights[9] = {
		{self.cotton_ID,	30},
		{self.silver_ID,	20},
		{self.sugar_ID,		20},
		{self.copper_ID,	20},
		{self.crab_ID,		25},
		{self.pearls_ID,	25},
		{self.whale_ID,		25},
		{self.cocoa_ID,		10},
		{self.coconut_ID,	25},
		{self.truffles_ID,	05},
		{self.spices_ID,	05},
		{self.gems_ID,		20},	};
		
		self.luxury_fallback_weights = {
		{self.whale_ID,		10},
		{self.pearls_ID,	10},
		{self.gold_ID,		10},
		{self.marble_ID,	05},
		{self.silver_ID,	05},
		{self.gems_ID,		10},
		{self.ivory_ID,		05},
		{self.fur_ID,		10},
		{self.dye_ID,		05},
		{self.spices_ID,	05},
		{self.silk_ID,		05},
		{self.sugar_ID,		05},
		{self.cotton_ID,	05},
		{self.wine_ID,		05},
		{self.incense_ID,	05},
		{self.copper_ID,	05},
		{self.salt_ID,		05},
		{self.citrus_ID,	05},
		{self.truffles_ID,	05},
		{self.cocoa_ID,		05},
		{self.rubber_ID,	05},
		{self.coconut_ID,	05},
		{self.crab_ID,		10},	};

		self.luxury_city_state_weights = {
		{self.whale_ID,		5},
		{self.pearls_ID,	5},
		{self.gold_ID,		5},
		{self.marble_ID,	5},
		{self.silver_ID,	5},
		{self.gems_ID,		5},
		{self.ivory_ID,		5},
		{self.fur_ID,		10},
		{self.dye_ID,		20},
		{self.spices_ID,	20},
		{self.silk_ID,		20},
		{self.sugar_ID,		25},
		{self.cotton_ID,	20},
		{self.wine_ID,		20},
		{self.incense_ID,	25},
		{self.copper_ID,	5},
		{self.salt_ID,		5},
		{self.citrus_ID,	5},
		{self.truffles_ID,	5},
		{self.cocoa_ID,		5},
		{self.crab_ID,		5},	};
	end
end	

function AssignStartingPlots:__CustomInit()


end	

function AssignStartingPlots:ApplyHexAdjustment(x, y, plot_adjustments)

	local iW, iH = Map.GetGridSize();
	local adjusted_x, adjusted_y;
	if Map:IsWrapX() == true then
		adjusted_x = (x + plot_adjustments[1]) % iW;
	else
		adjusted_x = x + plot_adjustments[1];
	end
	if Map:IsWrapY() == true then
		adjusted_y = (y + plot_adjustments[2]) % iH;
	else
		adjusted_y = y + plot_adjustments[2];
	end
	return adjusted_x, adjusted_y;
end



function AssignStartingPlots:MeasureStartPlacementFertilityOfPlot(x, y, checkForCoastalLand)














	local plot = Map.GetPlot(x, y);
	local plotFertility = 0;
	local plotType = plot:GetPlotType();
	local terrainType = plot:GetTerrainType();
	local featureType = plot:GetFeatureType();

	if plotType == PlotTypes.PLOT_MOUNTAIN then
		plotFertility = -1;
	elseif terrainType == TerrainTypes.TERRAIN_SNOW then
		plotFertility = -2;
	elseif featureType == FeatureTypes.FEATURE_ICE then
		plotFertility = -1;
	elseif plotType == PlotTypes.PLOT_OCEAN then
		plotFertility = 2;
	elseif featureType == FeatureTypes.FEATURE_OASIS then
		plotFertility = 4;
	elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
		plotFertility = 4;
	else
		if terrainType == TerrainTypes.TERRAIN_GRASS then
			plotFertility = 3;
		elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
			plotFertility = 4;
		elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
			plotFertility = 1;
		elseif terrainType == TerrainTypes.TERRAIN_DESERT then
			plotFertility = -1;
		end
		if plotType == PlotTypes.PLOT_HILLS then

				plotFertility = plotFertility + 1;

				plotFertility = plotFertility + 1;

		end
		if featureType == FeatureTypes.FEATURE_FOREST then
			if terrainType == TerrainTypes.TERRAIN_TUNDRA then
				plotFertility = plotFertility + 0;
			else
				plotFertility = plotFertility + 0;
			end
		elseif featureType == FeatureTypes.FEATURE_JUNGLE then
			plotFertility = plotFertility - 1;
		elseif featureType == FeatureTypes.FEATURE_MARSH then
			plotFertility = plotFertility - 2;
		end
		if plot:IsRiverSide() or plot:IsFreshWater() then
			plotFertility = plotFertility + 1;
		end
		if checkForCoastalLand == true then
			if plot:IsCoastalLand() then
				plotFertility = plotFertility + 2;
			end
		end
	end

	return plotFertility
end

function AssignStartingPlots:MeasureStartPlacementFertilityInRectangle(iWestX, iSouthY, iWidth, iHeight)



	local areaFertilityTable = {};
	local areaFertilityCount = 0;
	local plotCount = iWidth * iHeight;
	for y = iSouthY, iSouthY + iHeight - 1 do
		for x = iWestX, iWestX + iWidth - 1 do
			local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false);
			table.insert(areaFertilityTable, plotFertility);
			areaFertilityCount = areaFertilityCount + plotFertility;
		end
	end


	return areaFertilityTable, areaFertilityCount, plotCount
end

function AssignStartingPlots:MeasureStartPlacementFertilityOfLandmass(iAreaID, iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY)





	local iW, iH = Map.GetGridSize()
	local xEnd, yEnd;



	if wrapsX then
		xEnd = iEastX + iW;
	else
		xEnd = iEastX;
	end
	if wrapsY then
		yEnd = iNorthY + iH;
	else
		yEnd = iNorthY;
	end

	local areaFertilityTable = {};
	local areaFertilityCount = 0;
	local plotCount = 0;
	for yLoop = iSouthY, yEnd do
		for xLoop = iWestX, xEnd do
			plotCount = plotCount + 1;
			local x = xLoop % iW;
			local y = yLoop % iH;
			local plot = Map.GetPlot(x, y);
			local thisPlotsArea = plot:GetArea()
			if thisPlotsArea ~= iAreaID then
				table.insert(areaFertilityTable, 0);
			else
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true);
				table.insert(areaFertilityTable, plotFertility);
				areaFertilityCount = areaFertilityCount + plotFertility;
			end
		end
	end
	


	return areaFertilityTable, areaFertilityCount, plotCount
end

function AssignStartingPlots:RemoveDeadRows(fertility_table, iWestX, iSouthY, iWidth, iHeight)



	local iW, iH = Map.GetGridSize()
	local adjusted_table = {};
	local adjusted_WestX;
	local adjusted_SouthY
	local adjusted_Width
	local adjusted_Height;
	

	local adjustSouth = 0;
	for y = 0, iHeight - 1 do
		local bKeepThisRow = false;
		for x = 0, iWidth - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisRow = true;
				break
			end
		end
		if bKeepThisRow == true then
			break
		else
			adjustSouth = adjustSouth + 1;
		end
	end


	local adjustNorth = 0;
	for y = iHeight - 1, 0, -1 do
		local bKeepThisRow = false;
		for x = 0, iWidth - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisRow = true;
				break
			end
		end
		if bKeepThisRow == true then
			break
		else
			adjustNorth = adjustNorth + 1;
		end
	end


	local adjustWest = 0;
	for x = 0, iWidth - 1 do
		local bKeepThisColumn = false;
		for y = 0, iHeight - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisColumn = true;
				break
			end
		end
		if bKeepThisColumn == true then
			break
		else
			adjustWest = adjustWest + 1;
		end
	end


	local adjustEast = 0;
	for x = iWidth - 1, 0, -1 do
		local bKeepThisColumn = false;
		for y = 0, iHeight - 1 do
			local i = y * iWidth + x + 1;
			if fertility_table[i] ~= 0 then
				bKeepThisColumn = true;
				break
			end
		end
		if bKeepThisColumn == true then
			break
		else
			adjustEast = adjustEast + 1;
		end
	end

	if adjustSouth > 0 or adjustNorth > 0 or adjustWest > 0 or adjustEast > 0 then

		adjusted_WestX = (iWestX + adjustWest) % iW;
		adjusted_SouthY = (iSouthY + adjustSouth) % iH;
		adjusted_Width = (iWidth - adjustWest) - adjustEast;
		adjusted_Height = (iHeight - adjustSouth) - adjustNorth;

		for y = 0, adjusted_Height - 1 do
			for x = 0, adjusted_Width - 1 do
				local i = (y + adjustSouth) * iWidth + (x + adjustWest) + 1;
				local plotFert = fertility_table[i];
				table.insert(adjusted_table, plotFert);
			end
		end

		print("-");
		print("Removed Dead Rows, West: ", adjustWest, " East: ", adjustEast);
		print("Removed Dead Rows, South: ", adjustSouth, " North: ", adjustNorth);
		print("-");
		print("Incoming values: ", iWestX, iSouthY, iWidth, iHeight);
		print("Outgoing values: ", adjusted_WestX, adjusted_SouthY, adjusted_Width, adjusted_Height);
		print("-");
		local incoming_index = table.maxn(fertility_table);
		local outgoing_index = table.maxn(adjusted_table);
		print("Size of incoming fertility table: ", incoming_index);
		print("Size of outgoing fertility table: ", outgoing_index);

		return adjusted_table, adjusted_WestX, adjusted_SouthY, adjusted_Width, adjusted_Height;
	
	else
		return fertility_table, iWestX, iSouthY, iWidth, iHeight;
	end
end

function AssignStartingPlots:DivideIntoRegions(iNumDivisions, fertility_table, rectangle_data_table)




















	
	local iNumDivides = 0;
	local iSubdivisions = 0;
	local bPrimeGreaterThanThree = false;
	local firstSubdivisions = 0;
	local laterSubdivisions = 0;


	if (iNumDivisions == 1) then

		local fAverageFertility = rectangle_data_table[6] / rectangle_data_table[7];
		table.insert(rectangle_data_table, fAverageFertility);


		table.insert(self.regionData, rectangle_data_table);

		local iNumberOfThisRegion = table.maxn(self.regionData);
		print("-");
		print("---------------------------------------------");
		print("Defined location of Start Region #", iNumberOfThisRegion);
		print("---------------------------------------------");
		print("-");

		return



	elseif (iNumDivisions > 1) then

		local iWidth = rectangle_data_table[3];
		local iHeight = rectangle_data_table[4];
		local bTaller = false;
		if iHeight > iWidth then
			bTaller = true;
		end


		if (iNumDivisions == 2) then
			iNumDivides = 2;
			iSubdivisions = 1;
		elseif (iNumDivisions == 3) then
			iNumDivides = 3;
			iSubdivisions = 1;
		









		elseif (iNumDivisions == 5) then
			bPrimeGreaterThanThree = true;
			chopPercent = 59.2;
			firstSubdivisions = 3;
			laterSubdivisions = 2;
		elseif (iNumDivisions == 7) then
			bPrimeGreaterThanThree = true;
			chopPercent = 42.2;
			firstSubdivisions = 3;
			laterSubdivisions = 4;
		elseif (iNumDivisions == 11) then
			bPrimeGreaterThanThree = true;
			chopPercent = 27;
			firstSubdivisions = 3;
			laterSubdivisions = 8;
		elseif (iNumDivisions == 13) then
			bPrimeGreaterThanThree = true;
			chopPercent = 38.1;
			firstSubdivisions = 5;
			laterSubdivisions = 8;
		elseif (iNumDivisions == 17) then
			bPrimeGreaterThanThree = true;
			chopPercent = 52.8;
			firstSubdivisions = 9;
			laterSubdivisions = 8;
		elseif (iNumDivisions == 19) then
			bPrimeGreaterThanThree = true;
			chopPercent = 36.7;
			firstSubdivisions = 7;
			laterSubdivisions = 12;



		elseif (iNumDivisions == 4) then
			iNumDivides = 2;
			iSubdivisions = 2;
		elseif (iNumDivisions == 6) then
			iNumDivides = 3;
			iSubdivisions = 2;
		elseif (iNumDivisions == 8) then
			iNumDivides = 2;
			iSubdivisions = 4;
		elseif (iNumDivisions == 9) then
			iNumDivides = 3;
			iSubdivisions = 3;
		elseif (iNumDivisions == 10) then
			iNumDivides = 2;
			iSubdivisions = 5;
		elseif (iNumDivisions == 12) then
			iNumDivides = 3;
			iSubdivisions = 4;
		elseif (iNumDivisions == 14) then
			iNumDivides = 2;
			iSubdivisions = 7;
		elseif (iNumDivisions == 15) then
			iNumDivides = 3;
			iSubdivisions = 5;
		elseif (iNumDivisions == 16) then
			iNumDivides = 2;
			iSubdivisions = 8;
		elseif (iNumDivisions == 18) then
			iNumDivides = 3;
			iSubdivisions = 6;
		elseif (iNumDivisions == 20) then
			iNumDivides = 2;
			iSubdivisions = 10;
		elseif (iNumDivisions == 21) then
			iNumDivides = 3;
			iSubdivisions = 7;
		elseif (iNumDivisions == 22) then
			iNumDivides = 2;
			iSubdivisions = 11;
		else
			print("Erroneous number of regional divisions : ", iNumDivisions);
		end



		if bPrimeGreaterThanThree then
			print("DivideIntoRegions: Uneven Division for handling prime numbers selected.");
			local results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, chopPercent);
			local first_section_fertility_table = results[1];
			local first_section_data_table = results[2];
			local second_section_fertility_table = results[3];
			local second_section_data_table = results[4];

			self:DivideIntoRegions(firstSubdivisions, first_section_fertility_table, first_section_data_table)
			self:DivideIntoRegions(laterSubdivisions, second_section_fertility_table, second_section_data_table)

		else
			if (iNumDivides == 2) then
				print("DivideIntoRegions: Divide in to Halves selected.");
				local results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, 49.5);
				local first_section_fertility_table = results[1];
				local first_section_data_table = results[2];
				local second_section_fertility_table = results[3];
				local second_section_data_table = results[4];

				self:DivideIntoRegions(iSubdivisions, first_section_fertility_table, first_section_data_table)
				self:DivideIntoRegions(iSubdivisions, second_section_fertility_table, second_section_data_table)

			elseif (iNumDivides == 3) then
				print("DivideIntoRegions: Divide in to Thirds selected.");
				local results = self:ChopIntoThreeRegions(fertility_table, rectangle_data_table, bTaller);
				local first_section_fertility_table = results[1];
				local first_section_data_table = results[2];
				local second_section_fertility_table = results[3];
				local second_section_data_table = results[4];
				local third_section_fertility_table = results[5];
				local third_section_data_table = results[6];

				self:DivideIntoRegions(iSubdivisions, first_section_fertility_table, first_section_data_table)
				self:DivideIntoRegions(iSubdivisions, second_section_fertility_table, second_section_data_table)
				self:DivideIntoRegions(iSubdivisions, third_section_fertility_table, third_section_data_table)

			else
				print("Invalid iNumDivides value (from DivideIntoRegions): must be 2 or 3.");
			end
		end
	end
end

function AssignStartingPlots:ChopIntoThreeRegions(fertility_table, rectangle_data_table, bTaller, chopPercent)
	print("-"); print("ChopIntoThree called.");

	local results = {};


	local initial_results = self:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, 33);

	local temptable = initial_results[1];
	table.insert(results, temptable); 





	local temptable = initial_results[2];
	table.insert(results, temptable);




	local second_section_fertility_table = initial_results[3]; 



	local second_section_data_table = initial_results[4];





	local bTallerForRemainder = false;
	local width = second_section_data_table[3];
	local height = second_section_data_table[4];
	if height > width then
		bTallerForRemainder = true;
	end


	local interim_results = self:ChopIntoTwoRegions(second_section_fertility_table, second_section_data_table, bTallerForRemainder, 48.5);
	table.insert(results, interim_results[1]); 
	table.insert(results, interim_results[2]); 
	table.insert(results, interim_results[3]); 
	table.insert(results, interim_results[4]); 








	return results
end

function AssignStartingPlots:ChopIntoTwoRegions(fertility_table, rectangle_data_table, bTaller, chopPercent)

















	local iW, iH = Map.GetGridSize()
	local iWestX = rectangle_data_table[1];
	local iSouthY = rectangle_data_table[2];
	local iRectWidth = rectangle_data_table[3];
	local iRectHeight = rectangle_data_table[4];
	local iAreaID = rectangle_data_table[5];
	local iTargetFertility = rectangle_data_table[6] * chopPercent / 100;
	



	local firstRegionWestX = iWestX;
	local firstRegionSouthY = iSouthY;

	local firstRegionWidth, firstRegionHeight;
	local secondRegionWestX, secondRegionSouthY, secondRegionWidth, secondRegionHeight;
	local iFirstRegionFertility = 0;
	local iSecondRegionFertility = 0;
	local region_one_fertility = {};
	local region_two_fertility = {};

	if (bTaller) then


		firstRegionWidth = iRectWidth;
		secondRegionWestX = iWestX;
		secondRegionWidth = iRectWidth;


		local reachedTargetRow = false;
		local rectY = 0;
		while reachedTargetRow == false do

			for rectX = 0, iRectWidth - 1 do
				local fertIndex = rectY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];

				iFirstRegionFertility = iFirstRegionFertility + plotFertility;


				table.insert(region_one_fertility, plotFertility);
			end
			if iFirstRegionFertility >= iTargetFertility then

				firstRegionHeight = rectY + 1;
				secondRegionSouthY = (iSouthY + rectY + 1) % iH;
				secondRegionHeight = iRectHeight - firstRegionHeight;
				reachedTargetRow = true;
				break
			else
				rectY = rectY + 1;
			end
		end
		

		print("Dividing along horizontal line between rows: ", secondRegionSouthY - 1, "-", secondRegionSouthY);
		


		for rectY = firstRegionHeight, iRectHeight - 1 do
			for rectX = 0, iRectWidth - 1 do
				local fertIndex = rectY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];

				iSecondRegionFertility = iSecondRegionFertility + plotFertility;


				table.insert(region_two_fertility, plotFertility);
			end
		end
				
	else


		firstRegionHeight = iRectHeight;
		secondRegionSouthY = iSouthY;
		secondRegionHeight = iRectHeight;
		






		local reachedTargetColumn = false;
		local rectX = 0;
		while reachedTargetColumn == false do

			for rectY = 0, iRectHeight - 1 do
				local fertIndex = rectY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];

				iFirstRegionFertility = iFirstRegionFertility + plotFertility;

			end
			if iFirstRegionFertility >= iTargetFertility then

				firstRegionWidth = rectX + 1;
				secondRegionWestX = (iWestX + rectX + 1) % iW;
				secondRegionWidth = iRectWidth - firstRegionWidth;
				reachedTargetColumn = true;
				break
			else
				rectX = rectX + 1;
			end
		end


		print("Dividing along vertical line between columns: ", secondRegionWestX - 1, "-", secondRegionWestX);



		for rectY = 0, iRectHeight - 1 do
			for rectX = firstRegionWidth, iRectWidth - 1 do
				local fertIndex = rectY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];

				iSecondRegionFertility = iSecondRegionFertility + plotFertility;


				table.insert(region_two_fertility, plotFertility);
			end
		end

		for rectY = 0, iRectHeight - 1 do
			for rectX = 0, firstRegionWidth - 1 do
				local fertIndex = rectY * iRectWidth + rectX + 1;
				local plotFertility = fertility_table[fertIndex];
				table.insert(region_one_fertility, plotFertility);
			end
		end
	end
	




	local FRFertT, FRWX, FRSY, FRWid, FRHei;
	FRFertT, FRWX, FRSY, FRWid, FRHei = self:RemoveDeadRows(region_one_fertility,
		firstRegionWestX, firstRegionSouthY, firstRegionWidth, firstRegionHeight);


	local SRFertT, SRWX, SRSY, SRWid, SRHei;
	SRFertT, SRWX, SRSY, SRWid, SRHei = self:RemoveDeadRows(region_two_fertility,
		secondRegionWestX, secondRegionSouthY, secondRegionWidth, secondRegionHeight);

	

	local firstPlots = FRWid * FRHei;
	local secondPlots = SRWid * SRHei;
	local region_one_data = {FRWX, FRSY, FRWid, FRHei, iAreaID, iFirstRegionFertility, firstPlots};
	local region_two_data = {SRWX, SRSY, SRWid, SRHei, iAreaID, iSecondRegionFertility, secondPlots};

	local outcome = {FRFertT, region_one_data, SRFertT, region_two_data};
	return outcome
end

function AssignStartingPlots:CustomOverride()


end















function AssignStartingPlots:GenerateRegions(args)
	print("Map Generation - Dividing the map in to Regions");









	local args = args or {};
	local iW, iH = Map.GetGridSize();
	self.method = args.method or self.method;
	self.start_locations = args.start_locations or 2;
	self.resource_setting = args.resources or 3;
	self.CoastLux = args.CoastLux or false;
	self.AllowInlandSea = args.AllowInlandSea or 1;
	self.NoCoastInland = args.NoCoastInland;
	self.BalancedCoastal = args.BalancedCoastal;
	self.MixedBias = args.MixedBias;


	self.iNumCivs, self.iNumCityStates, self.player_ID_list, self.bTeamGame, self.teams_with_major_civs, self.number_civs_per_team = GetPlayerAndTeamInfo()
	self.iNumCityStatesUnassigned = self.iNumCityStates;
	print("-"); print("Civs:", self.iNumCivs); print("City States:", self.iNumCityStates);

	if self.method == 1 then

		local biggest_area = Map.FindBiggestArea(False);
		local iAreaID = biggest_area:GetID();

		local landmass_data = ObtainLandmassBoundaries(iAreaID);
		local iWestX = landmass_data[1];
		local iSouthY = landmass_data[2];
		local iEastX = landmass_data[3];
		local iNorthY = landmass_data[4];
		local iWidth = landmass_data[5];
		local iHeight = landmass_data[6];
		local wrapsX = landmass_data[7];
		local wrapsY = landmass_data[8];
		



		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(iAreaID, 
		                                         iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);









		local rect_table = {iWestX, iSouthY, iWidth, iHeight, iAreaID, fertCount, plotCount};

		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table)

	
	elseif self.method == 3 or self.method == 4 then




		self.inhabited_WestX = args.iWestX or 0;
		self.inhabited_SouthY = args.iSouthY or 0;
		self.inhabited_Width = args.iWidth or iW;
		self.inhabited_Height = args.iHeight or iH;



		local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityInRectangle(self.inhabited_WestX, 
		                                         self.inhabited_SouthY, self.inhabited_Width, self.inhabited_Height)

		local rect_table = {self.inhabited_WestX, self.inhabited_SouthY, self.inhabited_Width, 
		                    self.inhabited_Height, -1, fertCount, plotCount};

		self:DivideIntoRegions(self.iNumCivs, fert_table, rect_table)

	
	else








		local best_areas = {};
		local globalFertilityOfLands = {};


		local iGlobalFertilityOfLands = 0;
		local iNumLandPlots = 0;
		local iNumLandAreas = 0;
		local land_area_IDs = {};
		local land_area_plots = {};
		local land_area_fert = {};

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				if not plot:IsWater() then
					iNumLandPlots = iNumLandPlots + 1;
					local iArea = plot:GetArea();
					local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, true);
					iGlobalFertilityOfLands = iGlobalFertilityOfLands + plotFertility;

					if TestMembership(land_area_IDs, iArea) == false then
						iNumLandAreas = iNumLandAreas + 1;
						table.insert(land_area_IDs, iArea);
						land_area_plots[iArea] = 1;
						land_area_fert[iArea] = plotFertility;
					else
						land_area_plots[iArea] = land_area_plots[iArea] + 1;
						land_area_fert[iArea] = land_area_fert[iArea] + plotFertility;
					end
				end
			end
		end
		





		print("* * * * * * * * * *");
		for AreaID, fert in pairs(land_area_fert) do
			print("Area ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *");

		




		local interim_table = {};
		for loop_index, data_entry in pairs(land_area_fert) do
			table.insert(interim_table, data_entry);
		end
		




		

		table.sort(interim_table);

		for AreaID, fert in ipairs(interim_table) do
			print("Interim Table ID " .. AreaID .. " has fertility of " .. fert);
		end
		print("* * * * * * * * * *");


		local iNumRelevantLandAreas = math.min(iNumLandAreas, self.iNumCivs);



		local best_areas = {};

		local end_of_interim_table = table.maxn(interim_table);

		local fertility_value_list = {};
		local fertility_value_tie = false;
		for tableConstructionLoop = end_of_interim_table, (end_of_interim_table - iNumRelevantLandAreas + 1), -1 do
			if TestMembership(fertility_value_list, interim_table[tableConstructionLoop]) == true then
				fertility_value_tie = true;
				print("*** WARNING: Fertility Value Tie exists! ***");
			else
				table.insert(fertility_value_list, interim_table[tableConstructionLoop]);
			end
		end

		if fertility_value_tie == false then
			for areaTestLoop = end_of_interim_table, (end_of_interim_table - iNumRelevantLandAreas + 1), -1 do
				for loop_index, AreaID in ipairs(land_area_IDs) do
					if interim_table[areaTestLoop] == land_area_fert[land_area_IDs[loop_index]] then
						table.insert(best_areas, AreaID);
						break
					end
				end
			end
		else
			local iNumUniqueFertValues = table.maxn(fertility_value_list);
			for fertLoop = 1, iNumUniqueFertValues do
				for AreaID, fert in pairs(land_area_fert) do
					if fert == fertility_value_list[fertLoop] then

						local best_areas_length = table.maxn(best_areas);
						if best_areas_length < iNumRelevantLandAreas then
							table.insert(best_areas, AreaID);
						else
							break
						end
					end
				end
			end
		end
				

		print("-"); print("--- Continental Division, Initial Readout ---"); print("-");
		print("- Global Fertility:", iGlobalFertilityOfLands);
		print("- Total Land Plots:", iNumLandPlots);
		print("- Total Areas:", iNumLandAreas);
		print("- Relevant Areas:", iNumRelevantLandAreas); print("-");



		print("* * * * * * * * * *");
		for area_loop, AreaID in ipairs(best_areas) do
			print("Area ID " .. AreaID .. " has fertility of " .. land_area_fert[AreaID]);
		end
		print("* * * * * * * * * *");



		local inhabitedAreaIDs = {};
		local numberOfCivsPerArea = table.fill(0, iNumRelevantLandAreas);
		for civToAssign = 1, self.iNumCivs do
			local bestRemainingArea;
			local bestRemainingFertility = 0;
			local bestAreaTableIndex;



			print("- - Searching landmasses in order to place Civ #", civToAssign); print("-");
			for area_loop, AreaID in ipairs(best_areas) do
				local thisLandmassCurrentFertility = land_area_fert[AreaID] / (1 + numberOfCivsPerArea[area_loop]);
				if thisLandmassCurrentFertility > bestRemainingFertility then
					bestRemainingArea = AreaID;
					bestRemainingFertility = thisLandmassCurrentFertility;
					bestAreaTableIndex = area_loop;

					print("- Found new candidate landmass with Area ID#:", bestRemainingArea, " with fertility of ", bestRemainingFertility);
				end
			end

			numberOfCivsPerArea[bestAreaTableIndex] = numberOfCivsPerArea[bestAreaTableIndex] + 1;
			if TestMembership(inhabitedAreaIDs, bestRemainingArea) == false then
				table.insert(inhabitedAreaIDs, bestRemainingArea);
			end
			print("Civ #", civToAssign, "has been assigned to Area#", bestRemainingArea); print("-");
		end
		print("-"); print("--- End of Initial Readout ---"); print("-");
		
		print("*** Number of Civs per Landmass - Table Readout ***");
		PrintContentsOfTable(numberOfCivsPerArea)
		print("--- End of Civs per Landmass readout ***"); print("-"); print("-");
				



		local iNumInhabitedLandmasses = table.maxn(inhabitedAreaIDs);
		for loop, currentLandmassID in ipairs(inhabitedAreaIDs) do

			local landmass_data = ObtainLandmassBoundaries(currentLandmassID);
			local iWestX = landmass_data[1];
			local iSouthY = landmass_data[2];
			local iEastX = landmass_data[3];
			local iNorthY = landmass_data[4];
			local iWidth = landmass_data[5];
			local iHeight = landmass_data[6];
			local wrapsX = landmass_data[7];
			local wrapsY = landmass_data[8];



			local fert_table, fertCount, plotCount = self:MeasureStartPlacementFertilityOfLandmass(currentLandmassID, 
		  	                                         iWestX, iEastX, iSouthY, iNorthY, wrapsX, wrapsY);

			local rect_table = {iWestX, iSouthY, iWidth, iHeight, currentLandmassID, fertCount, plotCount};

			iNumCivsOnThisLandmass = numberOfCivsPerArea[loop];
			if iNumCivsOnThisLandmass > 0 and iNumCivsOnThisLandmass <= 22 then
			

				print("-"); print("- Region #: ", loop);
				print("- Civs on this landmass: ", iNumCivsOnThisLandmass);
				print("- Area ID#: ", currentLandmassID);
				print("- Fertility: ", fertCount);
				print("- Plot Count: ", plotCount); print("-");

			
				self:DivideIntoRegions(iNumCivsOnThisLandmass, fert_table, rect_table)
			else
				print("Invalid number of civs assigned to a landmass: ", iNumCivsOnThisLandmass);
			end
		end


	end
	

	self:CustomOverride()
	
















end



function AssignStartingPlots:MeasureTerrainInRegions()
	local iW, iH = Map.GetGridSize();

	for region_loop, region_data_table in ipairs(self.regionData) do
		local iWestX = region_data_table[1];
		local iSouthY = region_data_table[2];
		local iWidth = region_data_table[3];
		local iHeight = region_data_table[4];
		local iAreaID = region_data_table[5];
		
		local totalPlots, areaPlots = 0, 0;
		local waterCount, flatlandsCount, hillsCount, peaksCount = 0, 0, 0, 0;
		local lakeCount, coastCount, oceanCount, iceCount = 0, 0, 0, 0;
		local grassCount, plainsCount, desertCount, tundraCount, snowCount = 0, 0, 0, 0, 0;
		local forestCount, jungleCount, marshCount, riverCount, floodplainCount, oasisCount = 0, 0, 0, 0, 0, 0;
		local coastalLandCount, nextToCoastCount = 0, 0;


		for region_loop_y = 0, iHeight - 1 do
			for region_loop_x = 0, iWidth - 1 do
				totalPlots = totalPlots + 1;
				local x = (region_loop_x + iWestX) % iW;
				local y = (region_loop_y + iSouthY) % iH;
				local plot = Map.GetPlot(x, y);
				local area_of_plot = plot:GetArea();

				local plotType = plot:GetPlotType()
				local terrainType = plot:GetTerrainType()
				local featureType = plot:GetFeatureType()
				

				if plotType == PlotTypes.PLOT_MOUNTAIN then
					peaksCount = peaksCount + 1;
				elseif plotType == PlotTypes.PLOT_OCEAN then
					waterCount = waterCount + 1;
					if terrainType == TerrainTypes.TERRAIN_COAST then
						if plot:IsLake() then
							lakeCount = lakeCount + 1;
						else
							coastCount = coastCount + 1;
						end
					else
						oceanCount = oceanCount + 1;
					end
					if featureType == FeatureTypes.FEATURE_ICE then
						iceCount = iceCount + 1;
					end

				else

					if (area_of_plot == iAreaID) or (iAreaID == -1) then
						areaPlots = areaPlots + 1;


						local i = iW * y + x + 1;
			

						if plotType == PlotTypes.PLOT_HILLS then
							hillsCount = hillsCount + 1;

							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiverSide() then
								riverCount = riverCount + 1;
							end


							if featureType == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif featureType == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif featureType == FeatureTypes.FEATURE_OASIS then
								oasisCount = oasisCount + 1;
							end
								
						else
							flatlandsCount = flatlandsCount + 1;
	
							if self.plotDataIsCoastal[i] then
								coastalLandCount = coastalLandCount + 1;
							elseif self.plotDataIsNextToCoast[i] then
								nextToCoastCount = nextToCoastCount + 1;
							end

							if plot:IsRiverSide() then
								riverCount = riverCount + 1;
							end
				
							if terrainType == TerrainTypes.TERRAIN_GRASS then
								grassCount = grassCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								plainsCount = plainsCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								desertCount = desertCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
								tundraCount = tundraCount + 1;
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								snowCount = snowCount + 1;
							end
				

							if featureType == FeatureTypes.FEATURE_FOREST then
								forestCount = forestCount + 1;
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then
								jungleCount = jungleCount + 1;
							elseif featureType == FeatureTypes.FEATURE_MARSH then
								marshCount = marshCount + 1;
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								floodplainCount = floodplainCount + 1;
							elseif featureType == FeatureTypes.FEATURE_OASIS then
								oasisCount = oasisCount + 1;
							end
						end
					end
				end
			end
		end
			

		local regionCounts = {
			totalPlots, areaPlots,
			waterCount, flatlandsCount, hillsCount, peaksCount,
			lakeCount, coastCount, oceanCount, iceCount,
			grassCount, plainsCount, desertCount, tundraCount, snowCount,
			forestCount, jungleCount, marshCount, riverCount, floodplainCount, oasisCount,
			coastalLandCount, nextToCoastCount
			}

		












			

		table.insert(self.regionTerrainCounts, regionCounts);
		

		print("-");
		print("--- Region Terrain Measurements for Region #", region_loop, "---");
		print("Total Plots: ", totalPlots);
		print("Area Plots: ", areaPlots);
		print("-");
		print("Mountains: ", peaksCount, " - Cannot belong to a landmass AreaID.");
		print("Total Water Plots: ", waterCount, " - Cannot belong to a landmass AreaID.");
		print("-");
		print("Lake Plots: ", lakeCount);
		print("Coast Plots: ", coastCount, " - Does not include Lakes.");
		print("Ocean Plots: ", oceanCount);
		print("Icebergs: ", iceCount);
		print("-");
		print("Flatlands: ", flatlandsCount);
		print("Hills: ", hillsCount);
		print("-");
		print("Grass Plots: ", grassCount);
		print("Plains Plots: ", plainsCount);
		print("Desert Plots: ", desertCount);
		print("Tundra Plots: ", tundraCount);
		print("Snow Plots: ", snowCount);
		print("-");
		print("Forest Plots: ", forestCount);
		print("Jungle Plots: ", jungleCount);
		print("Marsh Plots: ", marshCount);
		print("Flood Plains: ", floodplainCount);
		print("Oases: ", oasisCount);
		print("-");
		print("Plots Along Rivers: ", riverCount);
		print("Plots Along Oceans: ", coastalLandCount);
		print("Plots Next To Plots Along Oceans: ", nextToCoastCount);
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		
	end
end

function AssignStartingPlots:DetermineRegionTypes()















	for this_region, terrainCounts in ipairs(self.regionTerrainCounts) do



		local totalPlots = terrainCounts[1] ;
		local areaPlots = terrainCounts[2];
		local waterCount = terrainCounts[3];
		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		local peaksCount = terrainCounts[6];
		local lakeCount = terrainCounts[7];
		local coastCount = terrainCounts[8];
		local oceanCount = terrainCounts[9];
		local iceCount = terrainCounts[10];
		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];
		local desertCount = terrainCounts[13];
		local tundraCount = terrainCounts[14];
		local snowCount = terrainCounts[15];
		local forestCount = terrainCounts[16];
		local jungleCount = terrainCounts[17];
		local marshCount = terrainCounts[18];
		local riverCount = terrainCounts[19];
		local floodplainCount = terrainCounts[20];
		local oasisCount = terrainCounts[21];
		local coastalLandCount = terrainCounts[22];
		local nextToCoastCount = terrainCounts[23];

		print("----------------------------------------------- REGION TYPE CHECKS START -----------------------------------------------");
		print("--- Region Terrain Measurements for Region #", this_region, "---");
		print("Total Plots: ", totalPlots);
		print("Area Plots: ", areaPlots);
		print("-");
		print("Mountains: ", peaksCount, " - Cannot belong to a landmass AreaID.");
		print("Total Water Plots: ", waterCount, " - Cannot belong to a landmass AreaID.");
		print("-");
		print("Lake Plots: ", lakeCount);
		print("Coast Plots: ", coastCount, " - Does not include Lakes.");
		print("Ocean Plots: ", oceanCount);
		print("Icebergs: ", iceCount);
		print("-");
		print("Flatlands: ", flatlandsCount);
		print("Hills: ", hillsCount);
		print("-");
		print("Grass Plots: ", grassCount);
		print("Plains Plots: ", plainsCount);
		print("Desert Plots: ", desertCount);
		print("Tundra Plots: ", tundraCount);
		print("Snow Plots: ", snowCount);
		print("-");
		print("Forest Plots: ", forestCount);
		print("Jungle Plots: ", jungleCount);
		print("Marsh Plots: ", marshCount);
		print("Flood Plains: ", floodplainCount);
		print("Oases: ", oasisCount);
		print("-");
		print("Plots Along Rivers: ", riverCount);
		print("Plots Along Oceans: ", coastalLandCount);
		print("Plots Next To Plots Along Oceans: ", nextToCoastCount);
		print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");



		if self.method == 3 or self.method == 4 then
			areaPlots = flatlandsCount + hillsCount;
		end


		if (tundraCount + snowCount) >= areaPlots * 0.10 then
			table.insert(self.regionTypes, 1);
			print("-");
			print("Region #", this_region, " has been defined as a Tundra Region.");
		

		elseif (jungleCount >= areaPlots * 0.12) then
			table.insert(self.regionTypes, 2);
			print("-");
			print("Region #", this_region, " has been defined as a Jungle Region.");
		elseif (jungleCount >= areaPlots * 0.10) and (jungleCount + forestCount >= areaPlots * 0.24) then
			table.insert(self.regionTypes, 2);
			print("-");
			print("Region #", this_region, " has been defined as a Jungle Region.");
		

		elseif (forestCount >= areaPlots * 0.21) then
			table.insert(self.regionTypes, 3);
			print("-");
			print("Region #", this_region, " has been defined as a Forest Region.");
		elseif (forestCount >= areaPlots * 0.8) and (jungleCount + forestCount >= areaPlots * 0.3) then
			table.insert(self.regionTypes, 3);
			print("-");
			print("Region #", this_region, " has been defined as a Forest Region.");
		

		elseif (desertCount >= areaPlots * 0.15) then
			table.insert(self.regionTypes, 4);
			print("-");
			print("Region #", this_region, " has been defined as a Desert Region.");
			

		elseif ((marshCount) >= areaPlots * 0.11) or (marshCount >= 6) then
			table.insert(self.regionTypes, 9);
			print("-");
			print("Region #", this_region, " has been defined as a Wetlands Region.");

		elseif (hillsCount >= areaPlots * 0.37) then
			table.insert(self.regionTypes, 5);
			print("-");
			print("Region #", this_region, " has been defined as a Hills Region.");
			

		elseif (grassCount >= areaPlots * 0.20) and (grassCount * 0.7 > plainsCount) then
			table.insert(self.regionTypes, 7);
			print("-");
			print("Region #", this_region, " has been defined as a Grassland Region.");
		

		elseif (plainsCount >= areaPlots * 0.27) and (plainsCount * 0.8 > grassCount) then
			table.insert(self.regionTypes, 6);
			print("-");
			print("Region #", this_region, " has been defined as a Plains Region.");

		elseif ((grassCount + plainsCount + desertCount + tundraCount + snowCount + hillsCount + peaksCount) > areaPlots * 0.8) then
			table.insert(self.regionTypes, 8);
			print("-");
			print("Region #", this_region, " has been defined as a Hybrid Region.");

		else
			table.insert(self.regionTypes, 0);
			print("-");
			print("Region #", this_region, " has been defined as an Undefined Region.");
		
		end
	end






































































		




		
	













		

		


















		
		








				






				






				













			














					

































		









		









		





			





		











		















		




end

function AssignStartingPlots:PlaceImpactAndRipples(x, y)

































	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;

	local ripple_decider = Map.GetCustomOption(6);
	local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};	
	if ripple_decider == 1 then
		local ripple_values = {97, 95, 92, 89, 69, 57, 24, 15};
	end
	if ripple_decider == 2 then	
		local ripple_values = {97, 95, 92, 88, 83, 77, 70, 62, 51, 41, 30, 18};
	end
	if ripple_decider == 3 then	
		local ripple_values = {99, 98, 97, 89, 88, 83, 77, 70, 62, 51, 41, 30, 18, 12};
	end
	


	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local plot = Map.GetPlot(x, y);


	self:PlaceResourceImpact(x, y, 1, 0)
	self:PlaceResourceImpact(x, y, 2, 3)
	self:PlaceResourceImpact(x, y, 3, 3)
	self:PlaceResourceImpact(x, y, 4, 3)
	if plot:IsCoastalLand() then
		if self._lek_collide_coastals then
			self:PlaceResourceImpactCoastalMod(x, y, 5, 3, 4)
		else
			self:PlaceResourceImpact(x, y, 5, 5)
		end
	else
		self:PlaceResourceImpact(x, y, 5, 5)
	end

	self:PlaceResourceImpact(x, y, 6, 4)


	local impactPlotIndex = y * iW + x + 1;
	self.distanceData[impactPlotIndex] = impact_value;
	self.playerCollisionData[impactPlotIndex] = true;
	self.cityStateData[impactPlotIndex] = 1;



	for ripple_radius, ripple_value in ipairs(ripple_values) do





		local currentX = x - ripple_radius;
		local currentY = y;



		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do

                if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end

				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];

				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else

					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local ringPlotIndex = realY * iW + realX + 1;
					if self.distanceData[ringPlotIndex] > 0 then

						local stronger_value = math.max(self.distanceData[ringPlotIndex], ripple_value);

						local overlap_value = math.min(97, math.floor(stronger_value * 1.4));
						self.distanceData[ringPlotIndex] = overlap_value;
					else
						self.distanceData[ringPlotIndex] = ripple_value;
					end

					if ripple_radius <= 6 then
						self.cityStateData[ringPlotIndex] = 1;
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
end

function AssignStartingPlots:MeasureSinglePlot(x, y, region_type, distance_from_city)
	local data = table.fill(false, 5);











	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y);
	local plotType = plot:GetPlotType()
	local terrainType = plot:GetTerrainType()
	local featureType = plot:GetFeatureType()
	
	if plotType == PlotTypes.PLOT_MOUNTAIN then
		data[4] = true;
		return data
	elseif plotType == PlotTypes.PLOT_OCEAN then
		if featureType == FeatureTypes.FEATURE_ICE then
			data[4] = true;
		elseif plot:IsLake() then
			data[1] = true;
		elseif terrainType == TerrainTypes.TERRAIN_COAST then 
			data[1] = true;
			if self.method == 3 or self.method == 4 then
				data[3] = true;
			end
		end

		return data
	end

	if featureType == FeatureTypes.FEATURE_JUNGLE and distance_from_city ~= 0 then
		if plotType == PlotTypes.PLOT_HILLS then
			data[2] = true;
		end
		if region_type ~= 7 then
			data[1] = true;
			if region_type == 2 then
				data[3] = true;
			end
		end
		return data
	elseif featureType == FeatureTypes.FEATURE_FOREST and distance_from_city ~= 0 then
		data[2] = true;
		if plotType == PlotTypes.PLOT_HILLS then
			data[3] = true;
		else

			if terrainType ~= TerrainTypes.TERRAIN_TUNDRA then
				data[1] = true;
			end
		end

		return data
	elseif featureType == FeatureTypes.FEATURE_OASIS then
		data[1] = true;
		data[3] = true;
		return data
	elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
		data[1] = true;
		data[3] = true;
		return data
	elseif featureType == FeatureTypes.FEATURE_MARSH and distance_from_city ~= 0 then
		

		if region_type == 9 then
			data[3] = true;
		else
			data[4] = true;
		end

		return data
	end

	if plotType == PlotTypes.PLOT_HILLS then
		data[2] = true;
		data[3] = true;
		return data
	end
	

	if terrainType == TerrainTypes.TERRAIN_SNOW then
		data[4] = true;
		data[5] = true;
		return data
		
	elseif terrainType == TerrainTypes.TERRAIN_DESERT then
		if region_type == 4 then
			data[3] = true;
		else
			data[4] = true;
		end
		return data

	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
		if region_type == 1 then
			data[3] = true;
		else
			data[4] = true;
		end
		return data

	elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
		data[3] = true;
		if region_type == 1 or region_type == 4 or region_type == 5 or region_type == 6 or region_type == 8 then
			data[1] = true;
		end
		return data

	elseif terrainType == TerrainTypes.TERRAIN_GRASS then
		data[1] = true;
		data[3] = true;
		return data
	end


	print("Encountered non-standard terrain.");
	return data
end

function AssignStartingPlots:EvaluateCandidatePlot(plotIndex, region_type)
	local goodSoFar = true;
	local iW, iH = Map.GetGridSize();
	local x = (plotIndex - 1) % iW;
	local y = (plotIndex - x - 1) / iW;
	local plot = Map.GetPlot(x, y);
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local distance_bias = self.distanceData[plotIndex];
	local adjacentMountainCount = 0;
	local foodTotal, prodTotal, goodTotal, junkTotal, riverTotal, coastScore = 0, 0, 0, 0, 0, 0;
	local search_table = {};
	

	if self.plotDataIsCoastal[plotIndex] == true then
		coastScore = 40;
	end
		

	local innerRingScore = 0;
	local result = self:MeasureSinglePlot(x, y, region_type, 0)
	if result[2] then
		innerRingScore = innerRingScore + 4;
		if result[5] then
			innerRingScore = innerRingScore + 4;
		end
	end
	if result[3] then
		goodTotal = goodTotal + 1;
		if result[5] then
			goodTotal = goodTotal + 1;
		end
	end
	if plot:IsRiverSide() or plot:IsFreshWater() then
		riverTotal = riverTotal + 4;
	end
		

	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

			junkTotal = junkTotal + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY);
			local result = self:MeasureSinglePlot(searchX, searchY, region_type, 1)
			if result[4] then
				junkTotal = junkTotal + 1;
				if searchPlot:GetPlotType() == PlotTypes.PLOT_MOUNTAIN and adjacentMountainCount == 0 then

					adjacentMountainCount = adjacentMountainCount + 1;
				elseif result[5] then
					junkTotal = junkTotal + 1;
				end
			else
				if result[1] then
					foodTotal = foodTotal + 1;
					if result[5] then
						foodTotal = foodTotal + 1;
					end
				end
				if result[2] then
					prodTotal = prodTotal + 1;
					if result[5] then
						prodTotal = prodTotal + 1;
					end
				end
				if result[3] then
					goodTotal = goodTotal + 1;
					if result[5] then
						goodTotal = goodTotal + 1;
					end
				end
				if searchPlot:IsRiverSide() or plot:IsFreshWater() then
					riverTotal = riverTotal + 2;
				end
			end
		end
	end


	if foodTotal < self.minFoodInner then
		goodSoFar = false;
	elseif prodTotal < self.minProdInner then
		goodSoFar = false;
	elseif goodTotal < self.minGoodInner then
		goodSoFar = false;
	end


	local weightedFoodInner = {0, 8, 14, 19, 22, 24, 25};
	local foodResultInner = weightedFoodInner[foodTotal + 1];
	local weightedProdInner = {0, 10, 16, 20, 20, 12, 0};
	local prodResultInner = weightedProdInner[prodTotal + 1];
	local goodResultInner = goodTotal * 2;
	innerRingScore = innerRingScore + foodResultInner + prodResultInner + goodResultInner + riverTotal - (junkTotal * 3);
	

	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

			junkTotal = junkTotal + 1;
		else
			local result = self:MeasureSinglePlot(searchX, searchY, region_type, 2)
			if result[4] then
				junkTotal = junkTotal + 1;
				if result[5] then
					junkTotal = junkTotal + 1;
				end
			else
				if result[1] then
					foodTotal = foodTotal + 1;
					if result[5] then
						foodTotal = foodTotal + 1;
					end
				end
				if result[2] then
					prodTotal = prodTotal + 1;
					if result[5] then
						prodTotal = prodTotal + 1;
					end
				end
				if result[3] then
					goodTotal = goodTotal + 1;
					if result[5] then
						goodTotal = goodTotal + 1;
					end
				end
				local searchPlot = Map.GetPlot(searchX, searchY);
				if searchPlot:IsRiverSide() or plot:IsFreshWater() then
					riverTotal = riverTotal + 2;
				end
			end
		end
	end


	if foodTotal < self.minFoodMiddle then
		goodSoFar = false;
	elseif prodTotal < self.minProdMiddle then
		goodSoFar = false;
	elseif goodTotal < self.minGoodMiddle then
		goodSoFar = false;
	end
	

	local weightedFoodMiddle = {0, 2, 5, 10, 20, 25, 28, 30, 32, 34, 35};
	local foodResultMiddle = 35;
	if foodTotal < 10 then
		foodResultMiddle = weightedFoodMiddle[foodTotal + 1];
	end
	local weightedProdMiddle = {0, 10, 20, 25, 30, 35};
	local effectiveProdTotal = prodTotal;
	if foodTotal * 2 < prodTotal then
		effectiveProdTotal = math.ceil(foodTotal / 2);
	end
	local prodResultMiddle = 35;
	if effectiveProdTotal < 5 then
		prodResultMiddle = weightedProdMiddle[effectiveProdTotal + 1];
	end
	local goodResultMiddle = goodTotal * 2;
	local middleRingScore = foodResultMiddle + prodResultMiddle + goodResultMiddle + riverTotal - (junkTotal * 3);
	

	if isEvenY then
		search_table = self.thirdRingYIsEven;
	else
		search_table = self.thirdRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY;
		if wrapX then
			searchX = (x + plot_adjustments[1]) % iW;
		else
			searchX = x + plot_adjustments[1];
		end
		if wrapY then
			searchY = (y + plot_adjustments[2]) % iH;
		else
			searchY = y + plot_adjustments[2];
		end
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

			junkTotal = junkTotal + 1;
		else
			local result = self:MeasureSinglePlot(searchX, searchY, region_type, 3)
			if result[4] then
				junkTotal = junkTotal + 1;
				if result[5] then
					junkTotal = junkTotal + 1;
				end
			else
				if result[1] then
					foodTotal = foodTotal + 1;
					if result[5] then
						foodTotal = foodTotal + 1;
					end
				end
				if result[2] then
					prodTotal = prodTotal + 1;
					if result[5] then
						prodTotal = prodTotal + 1;
					end
				end
				if result[3] then
					goodTotal = goodTotal + 1;
					if result[5] then
						goodTotal = goodTotal + 1;
					end
				end
				local searchPlot = Map.GetPlot(searchX, searchY);
				if searchPlot:IsRiverSide() or plot:IsFreshWater() then
					riverTotal = riverTotal + 2;
				end
			end
		end
	end


	if foodTotal < self.minFoodOuter then
		goodSoFar = false;
	elseif prodTotal < self.minProdOuter then
		goodSoFar = false;
	elseif goodTotal < self.minGoodOuter then
		goodSoFar = false;
	end
	if junkTotal > self.maxJunk then
		goodSoFar = false;
	end


	local outerRingScore = foodTotal + prodTotal + goodTotal + riverTotal - (junkTotal * 2);
	local finalScore = innerRingScore + middleRingScore + outerRingScore + coastScore;


	if distance_bias > 0 then



		goodSoFar = false;
		finalScore = finalScore - math.floor(finalScore * distance_bias / 100);
	end







	
	return finalScore, goodSoFar
end

function AssignStartingPlots:IterateThroughCandidatePlotList(plot_list, region_type)




	local found_eligible = false;
	local bestPlotScore = -5000;
	local bestPlotIndex;
	local found_fallback = false;
	local bestFallbackScore = -5000;
	local bestFallbackIndex;

	for loop, plotIndex in ipairs(plot_list) do
		local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type)

		if meets_minimums == true then
			found_eligible = true;
			if score > bestPlotScore then
				bestPlotScore = score;
				bestPlotIndex = plotIndex;
			end
		else
			found_fallback = true;
			if score > bestFallbackScore then
				bestFallbackScore = score;
				bestFallbackIndex = plotIndex;
			end
		end
	end

	local election_results = {found_eligible, bestPlotScore, bestPlotIndex, found_fallback, bestFallbackScore, bestFallbackIndex};
	return election_results
end

function AssignStartingPlots:FindStart(region_number, NoCoast)
	
	print("No Coast: ", NoCoast);
	


	local bSuccessFlag = false;
	local bForcedPlacementFlag = false;


	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local iMembershipEastX = iWestX + iWidth - 1;
	local iMembershipNorthY = iSouthY + iHeight - 1;

	local terrainCounts = self.regionTerrainCounts[region_number];

	local region_type = self.regionTypes[region_number];


	local fallback_plots = {};
	

	local fCenterWidth = (self.centerBias / 100) * iWidth;
	local iNonCenterWidth = math.floor((iWidth - fCenterWidth) / 2)
	local iCenterWidth = iWidth - (iNonCenterWidth * 2);
	local iCenterWestX = (iWestX + iNonCenterWidth) % iW;
	local iCenterTestWestX = (iWestX + iNonCenterWidth);
	local iCenterTestEastX = (iCenterWestX + iCenterWidth - 1);

	local fCenterHeight = (self.centerBias / 100) * iHeight;
	local iNonCenterHeight = math.floor((iHeight - fCenterHeight) / 2)
	local iCenterHeight = iHeight - (iNonCenterHeight * 2);
	local iCenterSouthY = (iSouthY + iNonCenterHeight) % iH;
	local iCenterTestSouthY = (iSouthY + iNonCenterHeight);
	local iCenterTestNorthY = (iCenterTestSouthY + iCenterHeight - 1);


	local fMiddleWidth = (self.middleBias / 100) * iWidth;
	local iOuterWidth = math.floor((iWidth - fMiddleWidth) / 2)
	local iMiddleWidth = iWidth - (iOuterWidth * 2);
	local iMiddleWestX = (iWestX + iOuterWidth) % iW;
	local iMiddleTestWestX = (iWestX + iOuterWidth);
	local iMiddleTestEastX = (iMiddleTestWestX + iMiddleWidth - 1);

	local fMiddleHeight = (self.middleBias / 100) * iHeight;
	local iOuterHeight = math.floor((iHeight - fMiddleHeight) / 2)
	local iMiddleHeight = iHeight - (iOuterHeight * 2);
	local iMiddleSouthY = (iSouthY + iOuterHeight) % iH;
	local iMiddleTestSouthY = (iSouthY + iOuterHeight);
	local iMiddleTestNorthY = (iMiddleTestSouthY + iMiddleHeight - 1); 


	local two_plots_from_ocean = {};
	local three_plots_from_ocean = {};
	local four_plots_from_ocean = {};
	local center_candidates = {};
	local center_river = {};
	local center_coastal = {};
	local center_inland_dry = {};
	local middle_candidates = {};
	local middle_river = {};
	local middle_coastal = {};
	local middle_inland_dry = {};
	local outer_plots = {};
	

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW;
			local y = (region_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then

				if self.plotDataIsNextToCoast[plotIndex] == true then
					table.insert(two_plots_from_ocean, plotIndex);
				elseif self.plotDataIsThreeFromCoast[plotIndex] == true then
					table.insert(three_plots_from_ocean, plotIndex);
				else
					local area_of_plot = plot:GetArea();
					if area_of_plot == iAreaID or iAreaID == -1 then



						local test_x = region_x + iWestX;
						local test_y = region_y + iSouthY;
						if (test_x >= iCenterTestWestX and test_x <= iCenterTestEastX) and 
						   (test_y >= iCenterTestSouthY and test_y <= iCenterTestNorthY) then
							
							if NoCoast == true and self.plotDataIsCoastal[plotIndex] == true then

							elseif plot:IsRiverSide() then
								table.insert(center_river, plotIndex);
								table.insert(center_candidates, plotIndex);
							elseif plot:IsFreshWater() or self.plotDataIsCoastal[plotIndex] == true then
								table.insert(center_coastal, plotIndex);
								table.insert(center_candidates, plotIndex);
							else
								table.insert(center_inland_dry, plotIndex);
								table.insert(center_candidates, plotIndex);
							end
							
						elseif (test_x >= iMiddleTestWestX and test_x <= iMiddleTestEastX) and 
						       (test_y >= iMiddleTestSouthY and test_y <= iMiddleTestNorthY) then
							
							if NoCoast == true and self.plotDataIsCoastal[plotIndex] == true then

							elseif plot:IsRiverSide() then
								table.insert(middle_river, plotIndex);
								table.insert(middle_candidates, plotIndex);
							elseif plot:IsFreshWater() or self.plotDataIsCoastal[plotIndex] == true then
								table.insert(middle_coastal, plotIndex);
								table.insert(middle_candidates, plotIndex);
							else
								table.insert(middle_inland_dry, plotIndex);
								table.insert(middle_candidates, plotIndex);
							end
						else
							if NoCoast == true and self.plotDataIsCoastal[plotIndex] == true then

							else
								table.insert(outer_plots, plotIndex);
							end
						end
					end
				end
			end
		end
	end

	

	local iNumDisqualified = table.maxn(two_plots_from_ocean) + table.maxn(three_plots_from_ocean);
	local iNumCenter = table.maxn(center_candidates);
	local iNumCenterRiver = table.maxn(center_river);
	local iNumCenterCoastLake = table.maxn(center_coastal);
	local iNumCenterInlandDry = table.maxn(center_inland_dry);
	local iNumMiddle = table.maxn(middle_candidates);
	local iNumMiddleRiver = table.maxn(middle_river);
	local iNumMiddleCoastLake = table.maxn(middle_coastal);
	local iNumMiddleInlandDry = table.maxn(middle_inland_dry);
	local iNumOuter = table.maxn(outer_plots);
	

	print("-");
	print("--- Number of Candidate Plots in Region #", region_number, " - Region Type:", region_type, " ---");
	print("-");
	print("Center Of Region at: " .. tostring(iCenterWestX) .. "," .. tostring(iCenterSouthY));
	print("-");
	print("Candidates in Center Bias area: ", iNumCenter);
	print("Which are next to river: ", iNumCenterRiver);
	print("Which are next to lake or sea: ", iNumCenterCoastLake);
	print("Which are inland and dry: ", iNumCenterInlandDry);
	print("-");
	print("Candidates in Middle Donut area: ", iNumMiddle);
	print("Which are next to river: ", iNumMiddleRiver);
	print("Which are next to lake or sea: ", iNumMiddleCoastLake);
	print("Which are inland and dry: ", iNumMiddleInlandDry);
	print("-");
	print("Candidate Plots in Outer area: ", iNumOuter);
	print("-");
	print("Disqualified, two or three plots away from salt water: ", iNumDisqualified);
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	
	

	if iNumCenter + iNumMiddle > 0 then
		if self._lek_prioritize_center then
			print("DEV/SAPHT Using _lek_prioritize_center")
			local candidate_lists = {};
			if iNumCenterRiver > 0 then
				table.insert(candidate_lists, center_river);
			end
			if iNumCenterCoastLake > 0 then
				table.insert(candidate_lists, center_coastal);
			end
			if iNumCenterInlandDry > 0 then
				table.insert(candidate_lists, center_inland_dry);
			end

			for loop, plot_list in ipairs(candidate_lists) do
				local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type)

				local found_eligible = election_returns[1];
				if found_eligible then
					local bestPlotScore = election_returns[2]; 
					local bestPlotIndex = election_returns[3];
					local x = (bestPlotIndex - 1) % iW;
					local y = (bestPlotIndex - x - 1) / iW;
					self.startingPlots[region_number] = {x, y, bestPlotScore};
					self:PlaceImpactAndRipples(x, y)
					return true, false
				end

				local found_fallback = election_returns[4];
				if found_fallback then
					local bestFallbackScore = election_returns[5];
					local bestFallbackIndex = election_returns[6];
					local x = (bestFallbackIndex - 1) % iW;
					local y = (bestFallbackIndex - x - 1) / iW;
					table.insert(fallback_plots, {x, y, bestFallbackScore});
				end
			end

			candidate_lists = {}
			if iNumMiddleRiver > 0 then
				table.insert(candidate_lists, middle_river);
			end
			if iNumMiddleCoastLake > 0 then
				table.insert(candidate_lists, middle_coastal);
			end
			if iNumMiddleInlandDry > 0 then
				table.insert(candidate_lists, middle_inland_dry);
			end

			for loop, plot_list in ipairs(candidate_lists) do
				local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type)

				local found_eligible = election_returns[1];
				if found_eligible then
					local bestPlotScore = election_returns[2]; 
					local bestPlotIndex = election_returns[3];
					local x = (bestPlotIndex - 1) % iW;
					local y = (bestPlotIndex - x - 1) / iW;
					self.startingPlots[region_number] = {x, y, bestPlotScore};
					self:PlaceImpactAndRipples(x, y)
					return true, false
				end

				local found_fallback = election_returns[4];
				if found_fallback then
					local bestFallbackScore = election_returns[5];
					local bestFallbackIndex = election_returns[6];
					local x = (bestFallbackIndex - 1) % iW;
					local y = (bestFallbackIndex - x - 1) / iW;
					table.insert(fallback_plots, {x, y, bestFallbackScore});
				end
			end
		else
			print("DEV/SAPHT Using default center priority")
			local candidate_lists = {};
			if iNumCenterRiver > 0 then
				table.insert(candidate_lists, center_river);
			end
			if iNumCenterCoastLake > 0 then
				table.insert(candidate_lists, center_coastal);
			end
			if iNumCenterInlandDry > 0 then
				table.insert(candidate_lists, center_inland_dry);
			end
			if iNumMiddleRiver > 0 then
				table.insert(candidate_lists, middle_river);
			end
			if iNumMiddleCoastLake > 0 then
				table.insert(candidate_lists, middle_coastal);
			end
			if iNumMiddleInlandDry > 0 then
				table.insert(candidate_lists, middle_inland_dry);
			end

			for loop, plot_list in ipairs(candidate_lists) do
				local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type)

				local found_eligible = election_returns[1];
				if found_eligible then
					local bestPlotScore = election_returns[2]; 
					local bestPlotIndex = election_returns[3];
					local x = (bestPlotIndex - 1) % iW;
					local y = (bestPlotIndex - x - 1) / iW;
					self.startingPlots[region_number] = {x, y, bestPlotScore};
					self:PlaceImpactAndRipples(x, y)
					return true, false
				end

				local found_fallback = election_returns[4];
				if found_fallback then
					local bestFallbackScore = election_returns[5];
					local bestFallbackIndex = election_returns[6];
					local x = (bestFallbackIndex - 1) % iW;
					local y = (bestFallbackIndex - x - 1) / iW;
					table.insert(fallback_plots, {x, y, bestFallbackScore});
				end
			end
		end
	end

	

	if iNumOuter > 0 then
		local outer_eligible_list = {};
		local found_eligible = false;
		local found_fallback = false;
		local bestFallbackScore = -50;
		local bestFallbackIndex;

		for loop, plotIndex in ipairs(outer_plots) do
			local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type)

			if meets_minimums == true then
				found_eligible = true;
				table.insert(outer_eligible_list, plotIndex);
			else
				found_fallback = true;
				if score > bestFallbackScore then
					bestFallbackScore = score;
					bestFallbackIndex = plotIndex;
				end
			end
		end
		if found_eligible then
			local closestPlot;
			local closestDistance = math.max(iW, iH);
			local bullseyeX = iWestX + (iWidth / 2);
			if bullseyeX < iWestX then
				bullseyeX = bullseyeX + iW;
			end
			local bullseyeY = iSouthY + (iHeight / 2);
			if bullseyeY < iSouthY then
				bullseyeY = bullseyeY + iH;
			end
			if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then
				bullseyeX = bullseyeX + 0.5;
			end
			
			for loop, plotIndex in ipairs(outer_eligible_list) do
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local adjusted_x = x;
				local adjusted_y = y;
				if y / 2 ~= math.floor(y / 2) then
					adjusted_x = x + 0.5;
				end
				
				if x < iWestX then
					adjusted_x = adjusted_x + iW;
				end
				if y < iSouthY then
					adjusted_y = y + iH;
				end
				local fDistance = math.sqrt( (adjusted_x - bullseyeX)^2 + (adjusted_y - bullseyeY)^2 );
				if fDistance < closestDistance then
					closestPlot = plotIndex;
					closestDistance = fDistance;
				end
			end

			local x = (closestPlot - 1) % iW;
			local y = (closestPlot - x - 1) / iW;

			local score, meets_minimums = self:EvaluateCandidatePlot(closestPlot, region_type)

			self.startingPlots[region_number] = {x, y, score};
			self:PlaceImpactAndRipples(x, y)
			return true, false
		end

		if found_fallback then
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end

	


	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for loop, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end

		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y)
		bSuccessFlag = true;
	else


		local forcePlot = Map.GetPlot(iWestX, iSouthY);
		bSuccessFlag = true;
		bForcedPlacementFlag = true;
		forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
		self.startingPlots[region_number] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY)
	end

	return bSuccessFlag, bForcedPlacementFlag
end

function AssignStartingPlots:FindCoastalStart(region_number)


	local bSuccessFlag = false;
	local bForcedPlacementFlag = false;
	local AllowInlandSea = self.AllowInlandSea;


	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local iMembershipEastX = iWestX + iWidth - 1;
	local iMembershipNorthY = iSouthY + iHeight - 1;

	local terrainCounts = self.regionTerrainCounts[region_number];
	local coastalLandCount = terrainCounts[22];

	local region_type = self.regionTypes[region_number];


	local fallback_plots = {};
	

	if coastalLandCount < 3 then

		bSuccessFlag, bForcedPlacementFlag = self:FindStart(region_number, false)
		if bSuccessFlag == false then


			local forcePlot = Map.GetPlot(iWestX, iSouthY);
			bForcedPlacementFlag = true;
			forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
			forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
			self.startingPlots[region_number] = {iWestX, iSouthY, 0};
			self:PlaceImpactAndRipples(iWestX, iSouthY)
		end
		return bSuccessFlag, bForcedPlacementFlag
	end


	local fCenterWidth = (self.centerBias / 100) * iWidth;
	local iNonCenterWidth = math.floor((iWidth - fCenterWidth) / 2)
	local iCenterWidth = iWidth - (iNonCenterWidth * 2);
	local iCenterWestX = (iWestX + iNonCenterWidth) % iW;
	local iCenterTestWestX = (iWestX + iNonCenterWidth);
	local iCenterTestEastX = (iCenterWestX + iCenterWidth - 1);

	local fCenterHeight = (self.centerBias / 100) * iHeight;
	local iNonCenterHeight = math.floor((iHeight - fCenterHeight) / 2)
	local iCenterHeight = iHeight - (iNonCenterHeight * 2);
	local iCenterSouthY = (iSouthY + iNonCenterHeight) % iH;
	local iCenterTestSouthY = (iSouthY + iNonCenterHeight);
	local iCenterTestNorthY = (iCenterTestSouthY + iCenterHeight - 1);


	local fMiddleWidth = (self.middleBias / 100) * iWidth;
	local iOuterWidth = math.floor((iWidth - fMiddleWidth) / 2)
	local iMiddleWidth = iWidth - (iOuterWidth * 2);

	local iMiddleWestX = (iWestX + iOuterWidth) % iW;
	local iMiddleTestWestX = (iWestX + iOuterWidth);
	local iMiddleTestEastX = (iMiddleTestWestX + iMiddleWidth - 1);

	local fMiddleHeight = (self.middleBias / 100) * iHeight;
	local iOuterHeight = math.floor((iHeight - fMiddleHeight) / 2)
	local iMiddleHeight = iHeight - (iOuterHeight * 2);

	local iMiddleSouthY = (iSouthY + iOuterHeight) % iH;
	local iMiddleTestSouthY = (iSouthY + iOuterHeight);
	local iMiddleTestNorthY = (iMiddleTestSouthY + iMiddleHeight - 1); 


	local center_coastal_plots = {};
	local center_plots_on_river = {};
	local center_fresh_plots = {};
	local center_dry_plots = {};
	local middle_coastal_plots = {};
	local middle_plots_on_river = {};
	local middle_fresh_plots = {};
	local middle_dry_plots = {};
	local outer_coastal_plots = {};
	

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = (region_x + iWestX) % iW;
			local y = (region_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			if self.plotDataIsCoastal[plotIndex] == true then
				local plot = Map.GetPlot(x, y);
				local plotType = plot:GetPlotType()
				print("@@@@@@@@@@");
				if plot:IsCoastalLand(50) then print("Plot at ", x, y, " is a land tile adjacent to 50 tiles of ocean"); end
				if plotType ~= PlotTypes.PLOT_MOUNTAIN and (AllowInlandSea == 1 or plot:IsCoastalLand(50)) then
					local area_of_plot = plot:GetArea();
					if area_of_plot == iAreaID or iAreaID == -1 then



						local test_x = region_x + iWestX;
						local test_y = region_y + iSouthY;
						if (test_x >= iCenterTestWestX and test_x <= iCenterTestEastX) and 
						   (test_y >= iCenterTestSouthY and test_y <= iCenterTestNorthY) then
							table.insert(center_coastal_plots, plotIndex);
							if plot:IsRiverSide() then
								table.insert(center_plots_on_river, plotIndex);
							elseif plot:IsFreshWater() then
								table.insert(center_fresh_plots, plotIndex);
							else
								table.insert(center_dry_plots, plotIndex);
							end
						elseif (test_x >= iMiddleTestWestX and test_x <= iMiddleTestEastX) and 
						       (test_y >= iMiddleTestSouthY and test_y <= iMiddleTestNorthY) then
							table.insert(middle_coastal_plots, plotIndex);
							if plot:IsRiverSide() then
								table.insert(middle_plots_on_river, plotIndex);
							elseif plot:IsFreshWater() then
								table.insert(middle_fresh_plots, plotIndex);
							else
								table.insert(middle_dry_plots, plotIndex);
							end
						else
							table.insert(outer_coastal_plots, plotIndex);
						end
					end
				end
			end
		end
	end

	local iNumCenterCoastal = table.maxn(center_coastal_plots);
	local iNumCenterRiver = table.maxn(center_plots_on_river);
	local iNumCenterFresh = table.maxn(center_fresh_plots);
	local iNumCenterDry = table.maxn(center_dry_plots);
	local iNumMiddleCoastal = table.maxn(middle_coastal_plots);
	local iNumMiddleRiver = table.maxn(middle_plots_on_river);
	local iNumMiddleFresh = table.maxn(middle_fresh_plots);
	local iNumMiddleDry = table.maxn(middle_dry_plots);
	local iNumOuterCoastal = table.maxn(outer_coastal_plots);
	

	print("-");
	print("--- Number of Candidate Plots next to an ocean in Region #", region_number, " - Region Type:", region_type, " ---");
	print("-");
	print("Center Of Region at: " .. tostring(iCenterWestX) .. "," .. tostring(iCenterSouthY));
	print("-");
	print("Coastal Plots in Center Bias area: ", iNumCenterCoastal);
	print("Which are along rivers: ", iNumCenterRiver);
	print("Which are fresh water: ", iNumCenterFresh);
	print("Which are dry: ", iNumCenterDry);
	print("-");
	print("Coastal Plots in Middle Donut area: ", iNumMiddleCoastal);
	print("Which are along rivers: ", iNumMiddleRiver);
	print("Which are fresh water: ", iNumMiddleFresh);
	print("Which are dry: ", iNumMiddleDry);
	print("-");
	print("Coastal Plots in Outer area: ", iNumOuterCoastal);
	print("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");

	

	if iNumCenterCoastal + iNumMiddleCoastal > 0 then
		local candidate_lists = {};
		if iNumCenterRiver > 0 then
			table.insert(candidate_lists, center_plots_on_river);
		end
		if iNumCenterFresh > 0 then
			table.insert(candidate_lists, center_fresh_plots);
		end
		if iNumCenterDry > 0 then
			table.insert(candidate_lists, center_dry_plots);
		end
		if iNumMiddleRiver > 0 then
			table.insert(candidate_lists, middle_plots_on_river);
		end
		if iNumMiddleFresh > 0 then
			table.insert(candidate_lists, middle_fresh_plots);
		end
		if iNumMiddleDry > 0 then
			table.insert(candidate_lists, middle_dry_plots);
		end

		for loop, plot_list in ipairs(candidate_lists) do
			local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type)

			local found_eligible = election_returns[1];
			if found_eligible then
				local bestPlotScore = election_returns[2]; 
				local bestPlotIndex = election_returns[3];
				local x = (bestPlotIndex - 1) % iW;
				local y = (bestPlotIndex - x - 1) / iW;
				self.startingPlots[region_number] = {x, y, bestPlotScore};
				self:PlaceImpactAndRipples(x, y)
				return true, false
			end

			local found_fallback = election_returns[4];
			if found_fallback then
				local bestFallbackScore = election_returns[5];
				local bestFallbackIndex = election_returns[6];
				local x = (bestFallbackIndex - 1) % iW;
				local y = (bestFallbackIndex - x - 1) / iW;
				table.insert(fallback_plots, {x, y, bestFallbackScore});
			end
		end
	end

	

	if iNumOuterCoastal > 0 then
		local outer_eligible_list = {};
		local found_eligible = false;
		local found_fallback = false;
		local bestFallbackScore = -50;
		local bestFallbackIndex;

		for loop, plotIndex in ipairs(outer_coastal_plots) do
			local score, meets_minimums = self:EvaluateCandidatePlot(plotIndex, region_type)

			if meets_minimums == true then
				found_eligible = true;
				table.insert(outer_eligible_list, plotIndex);
			else
				found_fallback = true;
				if score > bestFallbackScore then
					bestFallbackScore = score;
					bestFallbackIndex = plotIndex;
				end
			end
		end

		if found_eligible then
			local closestPlot;
			local closestDistance = math.max(iW, iH);
			local bullseyeX = iWestX + (iWidth / 2);
			if bullseyeX < iWestX then
				bullseyeX = bullseyeX + iW;
			end
			local bullseyeY = iSouthY + (iHeight / 2);
			if bullseyeY < iSouthY then
				bullseyeY = bullseyeY + iH;
			end
			if bullseyeY / 2 ~= math.floor(bullseyeY / 2) then
				bullseyeX = bullseyeX + 0.5;
			end
			
			for loop, plotIndex in ipairs(outer_eligible_list) do
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local adjusted_x = x;
				local adjusted_y = y;
				if y / 2 ~= math.floor(y / 2) then
					adjusted_x = x + 0.5;
				end
				
				if x < iWestX then
					adjusted_x = adjusted_x + iW;
				end
				if y < iSouthY then
					adjusted_y = y + iH;
				end
				local fDistance = math.sqrt( (adjusted_x - bullseyeX)^2 + (adjusted_y - bullseyeY)^2 );
				if fDistance < closestDistance then
					closestPlot = plotIndex;
					closestDistance = fDistance;
				end
			end

			local x = (closestPlot - 1) % iW;
			local y = (closestPlot - x - 1) / iW;

			local score, meets_minimums = self:EvaluateCandidatePlot(closestPlot, region_type)

			self.startingPlots[region_number] = {x, y, score};
			self:PlaceImpactAndRipples(x, y)
			return true, false
		end

		if found_fallback then
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end

	



	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for loop, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end

		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y)
		bSuccessFlag = true;
	else

		bSuccessFlag, bForcedPlacementFlag = self:FindStart(region_number, false)
		if bSuccessFlag == false then


			local forcePlot = Map.GetPlot(iWestX, iSouthY);
			bSuccessFlag = false;
			bForcedPlacementFlag = true;
			forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
			forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
			forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
			self.startingPlots[region_number] = {iWestX, iSouthY, 0};
			self:PlaceImpactAndRipples(iWestX, iSouthY)
		end
	end

	return bSuccessFlag, bForcedPlacementFlag
end

function AssignStartingPlots:FindStartWithoutRegardToAreaID(region_number, bMustBeCoast)


	local bSuccessFlag = false;
	local bForcedPlacementFlag = false;
	

	local iW, iH = Map.GetGridSize();
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local iMembershipEastX = iWestX + iWidth - 1;
	local iMembershipNorthY = iSouthY + iHeight - 1;

	local region_type = self.regionTypes[region_number];
	local fallback_plots = {};



	local regionalFertilityOfLands = {};
	local iRegionalFertilityOfLands = 0;
	local iNumLandPlots = 0;
	local iNumLandAreas = 0;
	local land_area_IDs = {};
	local land_area_plots = {};
	local land_area_fert = {};
	local land_area_plot_lists = {};

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = region_x + iWestX;
			local y = region_y + iSouthY;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then
				iNumLandPlots = iNumLandPlots + 1;
				local iArea = plot:GetArea();
				local plotFertility = self:MeasureStartPlacementFertilityOfPlot(x, y, false);
				iRegionalFertilityOfLands = iRegionalFertilityOfLands + plotFertility;
				if TestMembership(land_area_IDs, iArea) == false then
					iNumLandAreas = iNumLandAreas + 1;
					table.insert(land_area_IDs, iArea);
					land_area_plots[iArea] = 1;
					land_area_fert[iArea] = plotFertility;
				else
					land_area_plots[iArea] = land_area_plots[iArea] + 1;
					land_area_fert[iArea] = land_area_fert[iArea] + plotFertility;
				end
			end
		end
	end


	for loop, areaID in ipairs(land_area_IDs) do
		land_area_plot_lists[areaID] = {};
	end

	for region_y = 0, iHeight - 1 do
		for region_x = 0, iWidth - 1 do
			local x = region_x + iWestX;
			local y = region_y + iSouthY;
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local plotType = plot:GetPlotType()
			if plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then
				local iArea = plot:GetArea();
				if self.plotDataIsCoastal[i] == true then
					table.insert(land_area_plot_lists[iArea], i);
				elseif bMustBeCoast == false and self.plotDataIsNextToCoast[i] == false and self.plotDataIsThreeFromCoast[i] == false then
					table.insert(land_area_plot_lists[iArea], i);
				end
			end
		end
	end
	
	local best_areas = {};
	local regionAreaListUnsorted = {};
	local regionAreaListSorted = {};
	for areaNum, fert in pairs(land_area_fert) do
		table.insert(regionAreaListUnsorted, {areaNum, fert});
		table.insert(regionAreaListSorted, fert);
	end
	table.sort(regionAreaListSorted);
	

	local iNumAreas = table.maxn(regionAreaListSorted);
	for area_order = iNumAreas, 1, -1 do
		for loop, data_pair in ipairs(regionAreaListUnsorted) do
			local unsorted_fert = data_pair[2];
			if regionAreaListSorted[area_order] == unsorted_fert then
				local unsorted_area_num = data_pair[1];
				table.insert(best_areas, unsorted_area_num);

				table.remove(regionAreaListUnsorted, loop);
				break
			end
		end
	end














	for loop, iAreaID in ipairs(best_areas) do
		local plot_list = land_area_plot_lists[iAreaID];
		local election_returns = self:IterateThroughCandidatePlotList(plot_list, region_type)

		local found_eligible = election_returns[1];
		if found_eligible then
			local bestPlotScore = election_returns[2]; 
			local bestPlotIndex = election_returns[3];
			local x = (bestPlotIndex - 1) % iW;
			local y = (bestPlotIndex - x - 1) / iW;
			self.startingPlots[region_number] = {x, y, bestPlotScore};
			self:PlaceImpactAndRipples(x, y)
			return true, false
		end

		local found_fallback = election_returns[4];
		if found_fallback then
			local bestFallbackScore = election_returns[5];
			local bestFallbackIndex = election_returns[6];
			local x = (bestFallbackIndex - 1) % iW;
			local y = (bestFallbackIndex - x - 1) / iW;
			table.insert(fallback_plots, {x, y, bestFallbackScore});
		end
	end



	local iNumFallbacks = table.maxn(fallback_plots);
	if iNumFallbacks > 0 then
		local best_fallback_score = 0
		local best_fallback_x;
		local best_fallback_y;
		for loop, plotData in ipairs(fallback_plots) do
			local score = plotData[3];
			if score > best_fallback_score then
				best_fallback_score = score;
				best_fallback_x = plotData[1];
				best_fallback_y = plotData[2];
			end
		end

		self.startingPlots[region_number] = {best_fallback_x, best_fallback_y, best_fallback_score};
		self:PlaceImpactAndRipples(best_fallback_x, best_fallback_y)
		bSuccessFlag = true;
	else


		local forcePlot = Map.GetPlot(iWestX, iSouthY);
		bSuccessFlag = false;
		bForcedPlacementFlag = true;
		forcePlot:SetPlotType(PlotTypes.PLOT_LAND, false, true);
		forcePlot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, true);
		forcePlot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
		self.startingPlots[region_number] = {iWestX, iSouthY, 0};
		self:PlaceImpactAndRipples(iWestX, iSouthY)
	end

	return bSuccessFlag, bForcedPlacementFlag
end

function AssignStartingPlots:ChooseLocations(args)
	print("Map Generation - Choosing Start Locations for Civilizations");
	local args = args or {};
	local iW, iH = Map.GetGridSize();
	local mustBeCoast = args.mustBeCoast or false;
	



	self.centerBias = args.centerBias or self.centerBias;
	self.middleBias = args.middleBias or self.middleBias;
	print(string.format("DEV/SAPHT Center bias %d, %d", self.centerBias, self.middleBias))
	self.minFoodInner = args.minFoodInner or self.minFoodInner;
	self.minProdInner = args.minProdInner or self.minProdInner;
	self.minGoodInner = args.minGoodInner or self.minGoodInner;
	self.minFoodMiddle = args.minFoodMiddle or self.minFoodMiddle;
	self.minProdMiddle = args.minProdMiddle or self.minProdMiddle;
	self.minGoodMiddle = args.minGoodMiddle or self.minGoodMiddle;
	self.minFoodOuter = args.minFoodOuter or self.minFoodOuter;
	self.minProdOuter = args.minProdOuter or self.minProdOuter;
	self.minGoodOuter = args.minGoodOuter or self.minGoodOuter;
	self.maxJunk = args.maxJunk or self.maxJunk;


	self:MeasureTerrainInRegions()
	

	self:DetermineRegionTypes()




	local regionAssignList = {};
	local averageFertilityListUnsorted = {};
	local averageFertilityListSorted = {};
	for i, region_data in ipairs(self.regionData) do
		local thisRegionAvgFert = region_data[8];
		table.insert(averageFertilityListUnsorted, {i, thisRegionAvgFert});
		table.insert(averageFertilityListSorted, thisRegionAvgFert);
	end

	table.sort(averageFertilityListSorted);

	local iNumRegions = table.maxn(averageFertilityListSorted);
	for region_order = 1, iNumRegions do
		for loop, data_pair in ipairs(averageFertilityListUnsorted) do
			local unsorted_fert = data_pair[2];
			if averageFertilityListSorted[region_order] == unsorted_fert then
				local unsorted_reg_num = data_pair[1];
				table.insert(regionAssignList, unsorted_reg_num);



				table.remove(averageFertilityListUnsorted, loop);
				break
			end
		end
	end



	
	print("<<<<<<<<<<<<<<<<<< START OF REGION MANIPLUATION >>>>>>>>>>>>>>>>>>>>>");
	
	local iNumCoastNeeded = 0;
	local iNumRiverCivs, iNumPriorityCivs = 0, 0;
	local priority_lists = {};
	local res_reg = table.fill(false, self.iNumCivs);
	local reg_still_active = {};
	
	for loop = 1, self.iNumCivs do
		table.insert(reg_still_active, loop);
	end
	
	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop];
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		print("Player", playerNum, "of Civ Type", civType);
		local bNeedsCoastalStart = CivNeedsCoastalStart(civType);

		if self.MixedBias and Map.Rand(100, "") >= 60 and CivNeedsPlaceFirstCoastalStart(civType) then
			bNeedsCoastalStart = false;
		end
		if bNeedsCoastalStart == true then
			print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastNeeded = iNumCoastNeeded + 1;
		else
			local bNeedsRiverStart = CivNeedsRiverStart(civType)
			if bNeedsRiverStart == true then
				print("- - - - - - - needs River Start!"); print("-");
				iNumRiverCivs = iNumRiverCivs + 1;
			else
				local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
				if iNumRegionPriority > 0 then
					print("- - - - - - - needs Region Priority!"); print("-");
					local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
					iNumPriorityCivs = iNumPriorityCivs + 1;
					priority_lists[playerNum] = table_of_this_civs_priority_needs;
				end
			end
		end
	end
	
	for regcount = 1, iNumRegions do
		print("Region #", regcount, " Is type: ", self.regionTypes[regcount]);
	end
	
	print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
	local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority, iNumReserved = 0, 0, 0, 0;
	local single_priority, multi_priority, fallback_priority = {}, {}, {};
	local single_sorted, multi_sorted = {}, {};

	for playerNum, priority_needs in pairs(priority_lists) do
		local len = table.maxn(priority_needs)
		if len == 1 then
			print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
			
			local found_reg = false;
			

			for regcount = 1, iNumRegions do
				if self.regionTypes[regcount] == priority_needs[1] and found_reg == false then	

					
					if res_reg[regcount] == false then
						print("Region match found for player #", playerNum, " Region #:", regcount);
						print("--");
						res_reg[regcount] = true;
						iNumReserved = iNumReserved + 1;
						found_reg = true;
						table.remove(reg_still_active, regcount);
					end
				end
			end
			

			if found_reg == false then
				local iPriorityType = priority_needs[1];
				local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, reg_still_active)
				print("Fallback region found for player #", playerNum, " Region #:", choose_this_region);
				res_reg[choose_this_region] = true;
				iNumReserved = iNumReserved + 1;
				table.remove(reg_still_active, choose_this_region);
			end
		else
			print("Player#", playerNum, "has multiple Region Priority, this many types:", len);



		end
	end

	if self.BalancedCoastal then
		iRoll = Map.Rand(100, "Roll for extra coast");
		local iNumCoastStart = iNumCoastNeeded;
		if iNumRegions == 6 then
			if iNumCoastStart == 0 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 20 and 1 or 0) + (iRoll >= 45 and 1 or 0) + (iRoll >= 95 and 1 or 0);
			end
			if iNumCoastStart == 1 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 90 and 1 or 0)
			end
			if iNumCoastStart == 2 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 90 and 1 or 0)
			end
		end
		
		if iNumRegions == 8 then
			if iNumCoastStart == 0 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 15 and 1 or 0) + (iRoll >= 35 and 1 or 0) + (iRoll >= 65 and 1 or 0) + (iRoll >= 85 and 1 or 0)
			end
			if iNumCoastStart == 1 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 10 and 1 or 0) + (iRoll >= 55 and 1 or 0) + (iRoll >= 85 and 1 or 0)
			end
			if iNumCoastStart == 2 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 35 and 1 or 0) + (iRoll >= 80 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
			end
			if iNumCoastStart == 3 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 60 and 1 or 0)  + (iRoll >= 90 and 1 or 0)
			end
			if iNumCoastStart == 4 then
				iNumCoastNeeded = iNumCoastNeeded + (iRoll >= 75 and 1 or 0)  + (iRoll >= 95 and 1 or 0)
			end
		end
		

		local i = 1;
		while iNumRegions - iNumReserved > iNumCoastNeeded and i <= 100 do
		iRoll = Map.Rand(iNumRegions, "Roll region number to clear");
			if res_reg[iRoll] then
				res_reg[iRoll] = false;
				iNumReserved = iNumReserved - 1;
			end
			i = i + 1;
		end
	end

	
	for assignIndex = 1, iNumRegions do
		local currentRegionNumber = regionAssignList[assignIndex];
		local bSuccessFlag = false;
		local bForcedPlacementFlag = false;
		
		print("Region #" .. currentRegionNumber);
		print("Num coastal still needed " .. tostring(iNumCoastNeeded));


		if res_reg[currentRegionNumber] == false and iNumCoastNeeded > 0 then

			bSuccessFlag, bForcedPlacementFlag = self:FindCoastalStart(currentRegionNumber)
			iNumCoastNeeded = iNumCoastNeeded - 1;
		else
			print("Don't Allow Spawning on Coast: " .. tostring(self.NoCoastInland));

			bSuccessFlag, bForcedPlacementFlag = self:FindStart(currentRegionNumber, self.NoCoastInland)
		end
		

		print("- - -");
		print("Start Plot for Region #", currentRegionNumber, " was successful: ", bSuccessFlag);
		print("Start Plot for Region #", currentRegionNumber, " was forced: ", bForcedPlacementFlag);		
	end













	




end



function AssignStartingPlots:AttemptToPlaceBonusResourceAtPlot(x, y, bAllowOasis, Fish_Count)


	local plot = Map.GetPlot(x, y);
	local maxNumGranary = 4
	local maxFishPlace = Fish_Count
	if plot == nil then

		return false
	end
	if plot:GetResourceType(-1) ~= -1 then

		return false
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_SNOW then

		return false
	end
	local featureType = plot:GetFeatureType()
	if featureType == FeatureTypes.FEATURE_OASIS then

		return false
	end
	local plotType = plot:GetPlotType()

	



	if plotType == PlotTypes.PLOT_OCEAN then
		if maxNumGranary > 0 and maxFishPlace > 0 then
			if terrainType == TerrainTypes.TERRAIN_COAST and featureType == FeatureTypes.NO_FEATURE then
				if plot:IsLake() == false then
					plot:SetResourceType(self.fish_ID, 1);
					print("Placed Fish.");
					self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
					maxFishPlace = maxFishPlace - 1;
					return true, false, true
				end
			end
		end
	end
	if featureType == FeatureTypes.FEATURE_JUNGLE then
		if maxNumGranary > 0 then
		plot:SetResourceType(self.banana_ID, 1);
		print("Placed Banana.");
		self.amounts_of_resources_placed[self.banana_ID + 1] = self.amounts_of_resources_placed[self.banana_ID + 1] + 1;
		maxNumGranary = maxNumGranary - 1;
		return true, false, false
		else
		return false
		end
	elseif featureType == FeatureTypes.FEATURE_FOREST then
		if maxNumGranary > 0 then
		plot:SetResourceType(self.deer_ID, 1);
		print("Placed Deer.");
		self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1; 
		maxNumGranary = maxNumGranary - 1;
		return true, false, false
		else
		return false
		end
	elseif featureType == FeatureTypes.FEATURE_FOREST then
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then

		if maxNumGranary > 0 then
			plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
			plot:SetResourceType(self.deer_ID, 1);
			print("Placed Deer xx.");
			self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			return false
		end

	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE and terrainType ~= TerrainTypes.TERRAIN_DESERT then
		plot:SetResourceType(self.sheep_ID, 1);
		print("Placed Sheep xx.");
		self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
		return true, false, false
		

	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
		local placethis = Map.Rand(100, "");
		if placethis < 50 then
			if maxNumGranary > 0 then	
				plot:SetResourceType(self.bison_ID, 1);
				print("Placed Bison.");
				self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		elseif placethis > 50 and placethis < 90 then
			if maxNumGranary > 0 then
				plot:SetResourceType(self.sheep_ID, 1);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				print("Placed Sheep.");
				self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then	
				plot:SetResourceType(self.stone_ID, 1);
				print("Placed Stone.");
				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
		local placethis = Map.Rand(100, "");
		if placethis < 67 then
			if maxNumGranary > 0 then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
				plot:SetResourceType(self.deer_ID, 1);
				print("Placed Deer xx.");
				self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then
				plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
				plot:SetResourceType(self.deer_ID, 1);
				plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
				print("Placed Deer xx.");
				self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_GRASS then
			plot:SetResourceType(self.cow_ID, 1);
			print("Placed Cow.");
			self.amounts_of_resources_placed[self.cow_ID + 1] = self.amounts_of_resources_placed[self.cow_ID + 1] + 1;
		return true, false, false
	

	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		local placethis = Map.Rand(100, "");
		if placethis < 75 then
			if maxNumGranary > 0 then
				plot:SetResourceType(self.wheat_ID, 1);
				print("Placed Wheat.");
				self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		else
			if maxNumGranary > 0 then
				plot:SetResourceType(self.bison_ID, 1);
				print("Placed Bison.");
				self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		if maxNumGranary > 0 then
			plot:SetResourceType(self.bison_ID, 1);
			print("Placed Bison.");
			self.amounts_of_resources_placed[self.bison_ID + 1] = self.amounts_of_resources_placed[self.bison_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			return false
		end

	elseif terrainType == TerrainTypes.TERRAIN_DESERT and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then

		local placethis = Map.Rand(100, "");
		if placethis < 25 then
			plot:SetResourceType(self.wheat_ID, 1);
			print("Placed Wheat.");
			self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
			return true, false, false
		elseif placethis > 25 and placethis < 75 then
			plot:SetResourceType(self.sheep_ID, 1);
			plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
			plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
			print("Placed Sheep.");
			self.amounts_of_resources_placed[self.sheep_ID + 1] = self.amounts_of_resources_placed[self.sheep_ID + 1] + 1;
			maxNumGranary = maxNumGranary - 1;
			return true, false, false
		else
			if maxNumGranary > 0 then
				plot:SetResourceType(self.stone_ID, 1);
				print("Placed Stone.");
				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				maxNumGranary = maxNumGranary - 1;
				return true, false, false
			else
				return false
			end
		end
		
		
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		plot:SetResourceType(self.cow_ID, 1);
		print("Placed Cow.");
		self.amounts_of_resources_placed[self.cow_ID + 1] = self.amounts_of_resources_placed[self.cow_ID + 1] + 1;
		return true, false, false
	elseif plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE and terrainType == TerrainTypes.TERRAIN_PLAINS then
		plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
		plot:SetResourceType(self.hardwood_ID, 1);
		print("Placed Hardwood.");
		self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
		return true, false, false
		

	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then

					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
					plot:SetResourceType(self.deer_ID, 1);
					print("Placed Deer.");
					self.amounts_of_resources_placed[self.deer_ID + 1] = self.amounts_of_resources_placed[self.deer_ID + 1] + 1;
					return true, false, false
	elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then

					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1);
					plot:SetResourceType(self.hardwood_ID, 1);
					print("Placed Hardwood.");
					self.amounts_of_resources_placed[self.hardwood_ID + 1] = self.amounts_of_resources_placed[self.hardwood_ID + 1] + 1;
					return true, false, false

	elseif terrainType == TerrainTypes.TERRAIN_DESERT and plotType == PlotTypes.PLOT_LAND and featureType == FeatureTypes.NO_FEATURE then 
		if plot:IsFreshWater() then

			plot:SetResourceType(self.wheat_ID, 1);
			print("Placed Wheat.");
			self.amounts_of_resources_placed[self.wheat_ID + 1] = self.amounts_of_resources_placed[self.wheat_ID + 1] + 1;
			return true, false, false
		elseif bAllowOasis then
					plot:SetFeatureType(FeatureTypes.FEATURE_OASIS, -1);
					print("Placed Oasis.");
					return true, true, false
		else
					print("Not allowed to place any more Oasis help at this site.");
		end
	end
	

	return false, false, false
end

function AssignStartingPlots:AttemptToPlaceHillsAtPlot(x, y)


	local plot = Map.GetPlot(x, y);
	if plot == nil then

		return false
	end
	if plot:GetResourceType(-1) ~= -1 then

		return false
	end
	local plotType = plot:GetPlotType()
	local featureType = plot:GetFeatureType();
	if plotType == PlotTypes.PLOT_OCEAN then

		return false
	elseif plot:IsRiverSide() then

		return false
	elseif featureType == FeatureTypes.FEATURE_FOREST then

		return false
	end	

	plot:SetPlotType(PlotTypes.PLOT_HILLS, false, true);
	plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
	return true
end

function AssignStartingPlots:AttemptToPlaceSmallStrategicAtPlot(x, y)


	local plot = Map.GetPlot(x, y);
	if plot == nil then

		return false
	end
	if plot:GetResourceType(-1) ~= -1 then

		return false
	end
	local plotType = plot:GetPlotType()
	local terrainType = plot:GetTerrainType()
	local featureType = plot:GetFeatureType()
	if plotType ~= PlotTypes.PLOT_LAND then

		return false
	elseif featureType == FeatureTypes.NO_FEATURE then
		if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
			local choice = self.horse_ID;
			local diceroll = Map.Rand(5, "Selection of Strategic Resource type - Start Normalization LUA");
			if diceroll > 3 then
				choice = self.iron_ID;

			else

			end
			plot:SetResourceType(choice, 2);
			self.amounts_of_resources_placed[choice + 1] = self.amounts_of_resources_placed[choice + 1] + 2;
		else
			plot:SetResourceType(self.iron_ID, 2);
			self.amounts_of_resources_placed[self.iron_ID + 1] = self.amounts_of_resources_placed[self.iron_ID + 1] + 2;

		end
		return true
	end

	return false
end

function AssignStartingPlots:AddStrategicBalanceResources(region_number)


	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = y * iW + x + 1;
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local iron_list, horse_list, oil_list, alum_list, coal_list, uran_list = {}, {}, {}, {}, {}, {};
	local iron_fallback, horse_fallback, oil_fallback, alum_fallback, coal_fallback, uran_fallback = {}, {}, {}, {}, {}, {};
	local radius = 3;
	local OilToPlace = 2;
	

	
	for ripple_radius = 1, radius do
		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;

					if plotType == PlotTypes.PLOT_HILLS then
						if ripple_radius < 3 then
							table.insert(iron_list, plotIndex)

						else
							table.insert(iron_fallback, plotIndex)

						end
						if terrainType ~= TerrainTypes.TERRAIN_SNOW and featureType == FeatureTypes.NO_FEATURE then
							table.insert(horse_fallback, plotIndex)
						end
					elseif plotType == PlotTypes.PLOT_LAND then
						if featureType == FeatureTypes.NO_FEATURE then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA or terrainType == TerrainTypes.TERRAIN_DESERT then
								if ripple_radius < 3 then
									table.insert(oil_list, plotIndex)
								else
									table.insert(oil_fallback, plotIndex)
								end
								table.insert(iron_fallback, plotIndex)
								table.insert(horse_fallback, plotIndex)
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS then
								if ripple_radius < 3 then
									table.insert(horse_list, plotIndex)
								else
									table.insert(horse_fallback, plotIndex)
								end
								table.insert(iron_fallback, plotIndex)
								table.insert(oil_fallback, plotIndex)
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								if ripple_radius < 3 then
									table.insert(oil_list, plotIndex)
								else
									table.insert(oil_fallback, plotIndex)
								end
							end
						elseif featureType == FeatureTypes.FEATURE_MARSH then		
							if ripple_radius < 3 then
								table.insert(oil_list, plotIndex)
							else
								table.insert(oil_fallback, plotIndex)
							end
							table.insert(iron_fallback, plotIndex)
						elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
							table.insert(horse_fallback, plotIndex)
							table.insert(oil_fallback, plotIndex)
						elseif featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then		
							table.insert(iron_fallback, plotIndex)
							table.insert(oil_fallback, plotIndex)
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end


	local radius = 4;
	for ripple_radius = 1, radius do
		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;

					if plotType == PlotTypes.PLOT_HILLS then
						if ripple_radius < 4 then
							table.insert(alum_list, plotIndex)
							table.insert(coal_list, plotIndex)
						else
							table.insert(alum_fallback, plotIndex)
							table.insert(coal_fallback, plotIndex)
						end
						if terrainType ~= TerrainTypes.TERRAIN_SNOW and featureType == FeatureTypes.NO_FEATURE then
							table.insert(horse_fallback, plotIndex)
						end
					elseif plotType == PlotTypes.PLOT_LAND then
						if featureType == FeatureTypes.NO_FEATURE then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA or terrainType == TerrainTypes.TERRAIN_DESERT then
								if ripple_radius < 4 then
									table.insert(coal_list, plotIndex)
									table.insert(alum_list, plotIndex)
									table.insert(oil_fallback, plotIndex)
								else
									table.insert(coal_fallback, plotIndex)
									table.insert(alum_fallback, plotIndex)
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS then
								if ripple_radius < 4 then
									table.insert(coal_list, plotIndex)
									table.insert(alum_list, plotIndex)
									table.insert(oil_fallback, plotIndex)
								else
									table.insert(alum_fallback, plotIndex)
									table.insert(coal_fallback, plotIndex)
								end
							elseif terrainType == TerrainTypes.TERRAIN_SNOW then
								if ripple_radius < 4 then
									table.insert(coal_list, plotIndex)
									table.insert(alum_list, plotIndex)
									table.insert(oil_fallback, plotIndex)
								else
									table.insert(alum_fallback, plotIndex)
									table.insert(coal_fallback, plotIndex)
								end
							end
						elseif featureType == FeatureTypes.FEATURE_MARSH then		
							if ripple_radius < 4 then
								table.insert(coal_list, plotIndex)
								table.insert(alum_list, plotIndex)
								table.insert(oil_fallback, plotIndex)
							else
								table.insert(alum_fallback, plotIndex)
								table.insert(coal_fallback, plotIndex)
							end
						elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
							table.insert(alum_fallback, plotIndex)
							table.insert(coal_fallback, plotIndex)
						elseif featureType == FeatureTypes.FEATURE_JUNGLE or featureType == FeatureTypes.FEATURE_FOREST then		
							table.insert(alum_fallback, plotIndex)
							table.insert(coal_fallback, plotIndex)
							table.insert(oil_fallback, plotIndex)
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end

	local radius = 6;
	for ripple_radius = 4, radius do
		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;

					if plotType == PlotTypes.PLOT_LAND then
						if featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
							if featureType ~= FeatureTypes.FEATURE_OASIS then
								if terrainType == TerrainTypes.TERRAIN_TUNDRA or terrainType == TerrainTypes.TERRAIN_DESERT or terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_SNOW then
									table.insert(uran_list, plotIndex)
									table.insert(uran_fallback, plotIndex)
								end
							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
	local shuf_list;
	local placed_iron, placed_horse, placed_oil, placed_alum, placed_coal, placed_uran = false, false, false, false, false, false;

	uran_amt = 1;

	if table.maxn(iron_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_list)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_iron = true;
		end
	end
	if table.maxn(horse_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_list)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			placed_horse = true;
		end
	end
	if table.maxn(oil_list) > 0 then
		shuf_list = GetShuffledCopyOfTable(oil_list)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.oil_ID, oil_amt, 2, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace == 0 then
			print("All Oil Placed First Attempt");
			placed_oil = true;
			OilToPlace = 0;
		else
			OilToPlace = 1;
		end
	end
	
	if self.start_locations == 5 or self.start_locations == 6 or self.start_locations == 1 or self.start_locations == 2 then
		if table.maxn(alum_list) > 0 then
			shuf_list = GetShuffledCopyOfTable(alum_list)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.aluminum_ID, alum_amt, 1, 1, 1, 0, 0, shuf_list);
			if iNumLeftToPlace == 0 then
				placed_alum = true;
			end
		end
	end
	
	if self.start_locations == 4 or self.start_locations == 6 or self.start_locations == 1  or self.start_locations == 2 then
		if table.maxn(coal_list) > 0 then
			shuf_list = GetShuffledCopyOfTable(coal_list)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.coal_ID, coal_amt, 1, 1, 1, 0, 0, shuf_list);
			if iNumLeftToPlace == 0 then
				placed_coal = true;
			end
		end
	end

	if self.start_locations == 2 then
		if table.maxn(uran_list) > 0 then
			shuf_list = GetShuffledCopyOfTable(uran_list)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.uranium_ID, uran_amt, 2, 1, 1, 0, 0, shuf_list);
			if iNumLeftToPlace == 0 then
				placed_uran = true;
			end
		end
	end



	if placed_iron == false and table.maxn(iron_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(iron_fallback)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.iron_ID, iron_amt, 1, 1, -1, 0, 0, shuf_list);
	end
	if placed_horse == false and table.maxn(horse_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(horse_fallback)
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.horse_ID, horse_amt, 1, 1, -1, 0, 0, shuf_list);
	end
	if placed_oil == false and table.maxn(oil_fallback) > 0 then
		shuf_list = GetShuffledCopyOfTable(oil_fallback)
		if OilToPlace == 1 then
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.oil_ID, oil_amt, 1, 1, -1, 0, 0, shuf_list);
		else
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.oil_ID, oil_amt, 2, 1, -1, 0, 0, shuf_list);
		end
		print("Fallback Used");
		if iNumLeftToPlace == 0 then
			print("All Oil Placed 2nd Attempt");
		else

		end
	end
	if self.start_locations == 5 or self.start_locations == 6 or self.start_locations == 1 or self.start_locations == 2 then
		if placed_alum == false and table.maxn(alum_fallback) > 0 then
			shuf_list = GetShuffledCopyOfTable(horse_fallback)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.aluminum_ID, alum_amt, 1, 1, 1, 0, 0, shuf_list);
		end
	end
	if self.start_locations == 4 or self.start_locations == 6 or self.start_locations == 1 or self.start_locations == 2 then
		if placed_coal == false and table.maxn(coal_fallback) > 0 then
			shuf_list = GetShuffledCopyOfTable(coal_fallback)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.coal_ID, coal_amt, 1, 1, 1, 0, 0, shuf_list);
		end
	end
	if self.start_locations == 2 then
		if placed_uran == false and table.maxn(uran_fallback) > 0 then
			shuf_list = GetShuffledCopyOfTable(uran_fallback)
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.uranium_ID, uran_amt, 2, 1, 1, 0, 0, shuf_list);
		end
	end
end

function AssignStartingPlots:AttemptToPlaceStoneAtGrassPlot(x, y)



	local plot = Map.GetPlot(x, y);
	if plot == nil then

		return false
	end
	if plot:GetResourceType(-1) ~= -1 then

		return false
	end
	local plotType = plot:GetPlotType()
	if plotType == PlotTypes.PLOT_LAND then
		local featureType = plot:GetFeatureType()
		if featureType == FeatureTypes.NO_FEATURE then
			local terrainType = plot:GetTerrainType()
			if terrainType == TerrainTypes.TERRAIN_GRASS then
				plot:SetResourceType(self.stone_ID, 1);

				self.amounts_of_resources_placed[self.stone_ID + 1] = self.amounts_of_resources_placed[self.stone_ID + 1] + 1;
				return true
			end
		end
	end
end

function AssignStartingPlots:NormalizeStartLocation(region_number)












	print("-------------------------------- NormalizeStartLocation started -------------------------------- ")
	local iW, iH = Map.GetGridSize();
	local start_point_data = self.startingPlots[region_number];
	local x = start_point_data[1];
	local y = start_point_data[2];
	local plot = Map.GetPlot(x, y);
	local plotIndex = y * iW + x + 1;
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local innerFourFood, innerThreeFood, innerTwoFood, innerHills, innerForest, innerOneHammer, innerOcean = 0, 0, 0, 0, 0, 0, 0;
	local outerFourFood, outerThreeFood, outerTwoFood, outerHills, outerForest, outerOneHammer, outerOcean = 0, 0, 0, 0, 0, 0, 0;
	local innerCanHaveBonus, outerCanHaveBonus, innerBadTiles, outerBadTiles = 0, 0, 0, 0;
	local iNumFoodBonusNeeded = 0;
	local iNumNativeTwoFoodFirstRing, iNumNativeTwoFoodSecondRing = 0, 0;
	local search_table = {};
	

	self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true)
	
	print("%%%%%%%%%%%%%%%% PLOT EVALUATION %%%%%%%%%%%%%%%%");
	print("Evaluation for region: ", region_number, "At Location: ", x, y);
	

	local alongOcean = false;
	local nextToLake = false;
	local isRiver = false;
	local nearRiver = false;
	local nearMountain = false;
	local forestCount, jungleCount = 0, 0;


	if self.plotDataIsCoastal[plotIndex] == true then
		alongOcean = true;
	end
	

	if plot:IsRiver() then
		isRiver = true;
	end










	local iNumGrass, iNumPlains = 0, 0;


	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

			innerBadTiles = innerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				local nearMountain = true;
				innerBadTiles = innerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					nextToLake = true;
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerTwoFood = innerTwoFood + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerOcean = innerOcean + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				end
			else
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					innerHills = innerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						iNumGrass = iNumGrass + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						iNumPlains = iNumPlains + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerFourFood = innerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then

							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then

						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					else
						innerBadTiles = innerBadTiles + 1;
					end
				else
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerThreeFood = innerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then

							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodFirstRing = iNumNativeTwoFoodFirstRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerBadTiles = innerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					else
						innerBadTiles = innerBadTiles + 1;
					end
				end
			end
		end
	end
				

	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
		local plot = Map.GetPlot(x, y);


		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then

			outerBadTiles = outerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				local nearMountain = true;
				outerBadTiles = outerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					else
						outerTwoFood = outerTwoFood + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					elseif terrainType == TerrainTypes.TERRAIN_COAST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						outerOcean = outerOcean + 1;
					end
				end
			else
				if featureType == FeatureTypes.FEATURE_JUNGLE then
					jungleCount = jungleCount + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif featureType == FeatureTypes.FEATURE_FOREST then
					forestCount = forestCount + 1;
				end
				if searchPlot:IsRiver() then
					nearRiver = true;
				end
				if plotType == PlotTypes.PLOT_HILLS then
					outerHills = outerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						iNumGrass = iNumGrass + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						iNumPlains = iNumPlains + 1;
					end
				elseif featureType == FeatureTypes.FEATURE_OASIS then
					innerThreeFood = innerThreeFood + 1;
					iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerFourFood = outerFourFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then

							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then

						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					else
						outerBadTiles = outerBadTiles + 1;
					end
				else
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerThreeFood = outerThreeFood + 1;
						iNumGrass = iNumGrass + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then

							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						end
						if featureType == FeatureTypes.NO_FEATURE then
							iNumNativeTwoFoodSecondRing = iNumNativeTwoFoodSecondRing + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
						iNumPlains = iNumPlains + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerOneHammer = outerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							outerForest = outerForest + 1;
						else
							outerBadTiles = outerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					else
						outerBadTiles = outerBadTiles + 1;
					end
				end
			end
		end
	end
	

	local innerHammerScore = (3 * innerHills) + innerForest + innerOneHammer;
	local outerHammerScore = (2 * outerHills) + outerForest + outerOneHammer;
	local earlyHammerScore = (2 * innerForest) + outerForest + innerOneHammer + outerOneHammer;
	
	print("Inner Hammer: ", innerHammerScore);
	print("Outer Hammer: ", outerHammerScore);
	print("Early Hammer: ", earlyHammerScore);
	

	if (outerHammerScore <= 14 and innerHammerScore <= 6) or innerHammerScore == 0 then
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
		end
		for attempt = 1, 6 do
			local plot_adjustments = randomized_first_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

			local placedHill = self:AttemptToPlaceHillsAtPlot(searchX, searchY);
			if placedHill == true then
				innerHammerScore = innerHammerScore + 4;
				print("Added hills next to hammer-poor start plot at ", x, y);
				break
			elseif attempt == 6 then
				print("FAILED to add hills next to hammer-poor start plot at ", x, y);
			end
		end
	end
	

	if self.start_locations == 3 or self.start_locations == 4 or self.start_locations == 5 or self.start_locations == 6 or self.start_locations == 1 or self.start_locations == 2 then
		self:AddStrategicBalanceResources(region_number)
	end
	

	if innerHammerScore <= 4 and earlyHammerScore < 10 then
		if isEvenY then
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		for attempt = 1, 12 do
			local plot_adjustments = randomized_second_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

			local placedStrategic = self:AttemptToPlaceSmallStrategicAtPlot(searchX, searchY);
			if placedStrategic == true then
				print("Added small horse / iron next to hammer-poor start plot at ", x, y);
				break
			elseif attempt == 12 then
				print("FAILED to add small strategic resource near hammer-poor start plot at ", x, y);
			end
		end
	end
	

	local innerFoodScore = (4 * innerFourFood) + (2 * innerThreeFood) + innerTwoFood;
	local outerFoodScore = (4 * outerFourFood) + (2 * outerThreeFood) + outerTwoFood;
	local totalFoodScore = innerFoodScore + outerFoodScore;
	local nativeTwoFoodTiles = iNumNativeTwoFoodFirstRing + iNumNativeTwoFoodSecondRing;


	print("Inner Food: ", innerFoodScore);
	print("Outer Food: ", outerFoodScore);
	print("Total Food: ", totalFoodScore);
	print("Native Two Food: ", nativeTwoFoodTiles);
	
	print("-");
	print("-- - Start Point in Region #", region_number, " has Food Score of ", totalFoodScore, " with rings of ", innerFoodScore, outerFoodScore);
		
	

	if totalFoodScore < 4 and innerFoodScore == 0 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 6 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 8 then
		iNumFoodBonusNeeded = 2;
	elseif totalFoodScore < 12 and innerFoodScore < 5 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 17 and innerFoodScore < 9 then
		iNumFoodBonusNeeded = 3;
	elseif nativeTwoFoodTiles < 2 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 24 and innerFoodScore < 11 then
		iNumFoodBonusNeeded = 3;
	elseif nativeTwoFoodTiles == 2 or iNumNativeTwoFoodFirstRing < 2 then
		iNumFoodBonusNeeded = 3;
	elseif nativeTwoFoodTiles > 10 or iNumNativeTwoFoodFirstRing > 3 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 20 then
		iNumFoodBonusNeeded = 3;
	end
	

	
	if self.start_locations == 1 or self.start_locations == 2 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 1;
	end
	



	
	
	

	
	if innerOneHammer >= 4 and outerOneHammer >= 8 and iNumFoodBonusNeeded >= 4 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
	end
	

	if innerHills <= 2 and iNumFoodBonusNeeded <= 3 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end 
	

	if innerForest >= 3 or outerForest >= 2 and iNumFoodBonusNeeded <= 1 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 1;
	end
	
	

	
	if innerBadTiles >= 2 and innerBadTiles <= 3 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 1;
	end
	
	if innerBadTiles >= 4 and innerBadTiles <= 5 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end

	if innerBadTiles >= 6 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 3;
	end
	

	if iNumFoodBonusNeeded <= 1 then
		iNumFoodBonusNeeded = iNumFoodBonusNeeded + 2;
	end

	if iNumFoodBonusNeeded >= 3 then
		iNumFoodBonusNeeded = 3
	end 
	
	
	
	print("Food Bonuses: ", iNumFoodBonusNeeded);
	


































































	if iNumFoodBonusNeeded > 0 then
		local maxBonusesPossible = innerCanHaveBonus + outerCanHaveBonus;


		print("Food-Poor start ", x, y, " needs ", iNumFoodBonusNeeded, " Bonus, with ", maxBonusesPossible, " eligible plots.");


		local innerPlaced, outerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments, randomized_third_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
			randomized_third_ring_adjustments = GetShuffledCopyOfTable(self.thirdRingYIsOdd);
		end

		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		local tried_all_third_ring = false;
		local allow_oasis = true;
		local allow_fishCount = 0;


		if self._lek_coastal_refish then
			allow_fishcount = 0;
		end

		local placedOasis;
		while iNumFoodBonusNeeded > 0 do
			if ((innerPlaced < 2 and innerCanHaveBonus > 0) or (self.start_locations == 1 and innerPlaced < 5 and innerCanHaveBonus > 0) or (self.start_locations == 2 and innerPlaced < 5 and innerCanHaveBonus > 0))
			  and tried_all_first_ring == false then

				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus, placedOasis, placedFish = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis, allow_fishCount);
					if placedBonus == true then
						if placedFish == true then
							allow_fishCount = allow_fishCount - 1;
						end
						if allow_oasis == true and placedOasis == true then
							allow_oasis = false;
						end

						innerPlaced = innerPlaced + 1;
						innerCanHaveBonus = innerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif ((innerPlaced + outerPlaced < 5 and outerCanHaveBonus > 0) or (self.start_locations == 1 and innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0) or (self.start_locations == 2 and innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0))
			  and tried_all_second_ring == false then

				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus, placedOasis, placedFish = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis, allow_fishCount);
					if placedBonus == true then
						if placedFish == true then
							allow_fishCount = allow_fishCount - 1;
						end
						if allow_oasis == true and placedOasis == true then
							allow_oasis = false;
						end

						outerPlaced = outerPlaced + 1;
						outerCanHaveBonus = outerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			elseif tried_all_third_ring == false then

				for attempt = 1, 18 do
					local plot_adjustments = randomized_third_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus, placedOasis, placedFish = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis, allow_fishCount);
					if placedBonus == true then
						if placedFish == true then
							allow_fishCount = allow_fishCount - 1;
						end
						if allow_oasis == true and placedOasis == true then
							allow_oasis = false;
						end

						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 18 then
						tried_all_third_ring = true;
					end
				end
				
			else
				break				
			end
		end
	end


	local iNumStoneNeeded = 0;
	if iNumGrass >= 7 and iNumPlains < 2 then
		iNumStoneNeeded = 2;
	elseif iNumGrass >= 6 and iNumPlains <= 4 then
		iNumStoneNeeded = 1;
	elseif iNumGrass >= 9 and iNumPlains <= 1 then
		iNumStoneNeeded = 3;
	end
	
	if iNumStoneNeeded > 0 then
		local stonePlaced, innerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		while iNumStoneNeeded > 0 do
			if innerPlaced < 1 and tried_all_first_ring == false then

				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus = self:AttemptToPlaceStoneAtGrassPlot(searchX, searchY);
					if placedBonus == true then

						innerPlaced = innerPlaced + 1;
						iNumStoneNeeded = iNumStoneNeeded - 1;
						break
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif tried_all_second_ring == false then

				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus = self:AttemptToPlaceStoneAtGrassPlot(searchX, searchY);
					if placedBonus == true then

						iNumStoneNeeded = iNumStoneNeeded - 1;
						break
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end

			else
				break				
			end
		end
	end
	

	local results_table = {alongOcean, nextToLake, isRiver, nearRiver, nearMountain, forestCount, jungleCount};
	self.startLocationConditions[region_number] = results_table;
end

function AssignStartingPlots:FindFallbackForUnmatchedRegionPriority(iRegionType, regions_still_available)



	local iMostTundra, iMostTundraForest, iMostJungle, iMostForest, iMostDesert = 0, 0, 0, 0, 0;
	local iMostHills, iMostPlains, iMostGrass, iMostHybrid, iMostWet = 0, 0, 0, 0, 0;
	local bestTundra, bestTundraForest, bestJungle, bestForest, bestDesert = -1, -1, -1, -1, -1;
	local bestHills, bestPlains, bestGrass, bestHybrid, bestWet = -1, -1, -1, -1, -1;

	for loop, region_number in ipairs(regions_still_available) do
		local terrainCounts = self.regionTerrainCounts[region_number];



		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		local peaksCount = terrainCounts[6];




		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];
		local desertCount = terrainCounts[13];
		local tundraCount = terrainCounts[14];
		local snowCount = terrainCounts[15];
		local forestCount = terrainCounts[16];
		local jungleCount = terrainCounts[17];
		local marshCount = terrainCounts[18];

		local floodplainCount = terrainCounts[20];
		local oasisCount = terrainCounts[21];


		
		if iRegionType == 1 then
			if tundraCount + snowCount > iMostTundra then
				bestTundra = region_number;
				iMostTundra = tundraCount + snowCount;
			end
			if forestCount > iMostTundraForest and jungleCount == 0 then
				bestTundraForest = region_number;
				iMostTundraForest = forestCount;
			end
		elseif iRegionType == 2 then
			if jungleCount > iMostJungle then
				bestJungle = region_number;
				iMostJungle = jungleCount;
			end
		elseif iRegionType == 3 then
			if forestCount > iMostForest then
				bestForest = region_number;
				iMostForest = forestCount;
			end
		elseif iRegionType == 4 then
			if desertCount + floodplainCount + oasisCount > iMostDesert then
				bestDesert = region_number;
				iMostDesert = desertCount + floodplainCount + oasisCount;
			end
		elseif iRegionType == 5 then
			if hillsCount + peaksCount > iMostHills then
				bestHills = region_number;
				iMostHills = hillsCount + peaksCount;
			end
		elseif iRegionType == 6 then
			if plainsCount > iMostPlains then
				bestPlains = region_number;
				iMostPlains = plainsCount;
			end
		elseif iRegionType == 7 then
			if grassCount + marshCount > iMostGrass then
				bestGrass = region_number;
				iMostGrass = grassCount + marshCount;
			end
		elseif iRegionType == 8 then
			if grassCount + plainsCount > iMostHybrid then
				bestHybrid = region_number;
				iMostHybrid = grassCount + plainsCount;
			end
		elseif iRegionType == 9 then
			if marshCount > iMostWet then
				bestWet = region_number;
				iMostWet = marshCount;
			end
		end
	end
	
	if iRegionType == 1 then
		if bestTundra ~= -1 then
			return bestTundra
		elseif bestTundraForest ~= -1 then
			return bestTundraForest
		end
	elseif iRegionType == 2 and bestJungle ~= -1 then
		return bestJungle
	elseif iRegionType == 3 and bestForest ~= -1 then
		return bestForest
	elseif iRegionType == 4 and bestDesert ~= -1 then
		return bestDesert
	elseif iRegionType == 5 and bestHills ~= -1 then
		return bestHills
	elseif iRegionType == 6 and bestPlains ~= -1 then
		return bestPlains
	elseif iRegionType == 7 and bestGrass ~= -1 then
		return bestGrass
	elseif iRegionType == 9 and bestGrass ~= -1 then
		return bestGrass
	elseif iRegionType == 8 and bestHybrid ~= -1 then
		return bestHybrid
	elseif iRegionType == 9 and bestWet ~= -1 then
		return bestWet
	end

	return -1
end

function AssignStartingPlots:NormalizeTeamLocations()



end

function AssignStartingPlots:BalanceAndAssign(args)




	local iW, iH = Map.GetGridSize();
	local iNumStarts = table.maxn(self.startingPlots);
	for region_number = 1, iNumStarts do
		self:NormalizeStartLocation(region_number)
	end



	local bDisableStartBias = Game.GetCustomOption("GAMEOPTION_DISABLE_START_BIAS");
	if bDisableStartBias == 1 then

		local playerList = {};
		for loop = 1, self.iNumCivs do
			local player_ID = self.player_ID_list[loop];
			table.insert(playerList, player_ID);
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for region_number, player_ID in ipairs(playerListShuffled) do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			local i = y * iW + x + 1;
			player:SetStartingPlot(start_plot)
		end



		if self.bTeamGame == true then
			self:NormalizeTeamLocations()
		end

		return
	end



	local all_regions = {};
	for loop = 1, self.iNumCivs do
		table.insert(all_regions, loop);
	end
	local regions_still_available = GetShuffledCopyOfTable(all_regions)

	local civs_needing_coastal_start = {};
	local civs_priority_coastal_start = {};
	local civs_needing_river_start = {};
	local civs_needing_region_priority = {};
	local civs_needing_region_avoid = {};
	local regions_with_coastal_start = {};
	local regions_with_lake_start = {};
	local regions_with_river_start = {};
	local regions_with_near_river_start = {};
	local civ_status = table.fill(false, GameDefines.MAX_MAJOR_CIVS);
	local region_status = table.fill(false, self.iNumCivs);
	local priority_lists = {};
	local avoid_lists = {};
	local iNumCoastalCivs, iNumRiverCivs, iNumPriorityCivs, iNumAvoidCivs = 0, 0, 0, 0;
	local iNumCoastalCivsRemaining, iNumRiverCivsRemaining, iNumPriorityCivsRemaining, iNumAvoidCivsRemaining = 0, 0, 0, 0;
	

	


	for loop = 1, self.iNumCivs do
		local playerNum = self.player_ID_list[loop];
		local player = Players[playerNum];
		local civType = GameInfo.Civilizations[player:GetCivilizationType()].Type;
		print("Player", playerNum, "of Civ Type", civType);
		local bNeedsCoastalStart = CivNeedsCoastalStart(civType)
		if args.MixedBias and Map.Rand(100, "") >= 0 and CivNeedsPlaceFirstCoastalStart(civType) then 
			bNeedsCoastalStart = false;
		end
		if bNeedsCoastalStart == true then
			print("- - - - - - - needs Coastal Start!"); print("-");
			iNumCoastalCivs = iNumCoastalCivs + 1;
			iNumCoastalCivsRemaining = iNumCoastalCivsRemaining + 1;
			table.insert(civs_needing_coastal_start, playerNum);
			local bPlaceFirst = CivNeedsPlaceFirstCoastalStart(civType);
			if bPlaceFirst then
				print("- - - - - - - needs to Place First!"); print("-");
				table.insert(civs_priority_coastal_start, playerNum);
			end
		else
			local bNeedsRiverStart = CivNeedsRiverStart(civType)
			if bNeedsRiverStart == true then

				iNumRiverCivs = iNumRiverCivs + 1;
				iNumRiverCivsRemaining = iNumRiverCivsRemaining + 1;
				table.insert(civs_needing_river_start, playerNum);
			else
				local iNumRegionPriority = GetNumStartRegionPriorityForCiv(civType)
				if iNumRegionPriority > 0 then

					local table_of_this_civs_priority_needs = GetStartRegionPriorityListForCiv_GetIDs(civType)
					iNumPriorityCivs = iNumPriorityCivs + 1;
					iNumPriorityCivsRemaining = iNumPriorityCivsRemaining + 1;
					table.insert(civs_needing_region_priority, playerNum);
					priority_lists[playerNum] = table_of_this_civs_priority_needs;
				else
					local iNumRegionAvoid = GetNumStartRegionAvoidForCiv(civType)
					if iNumRegionAvoid > 0 then

						local table_of_this_civs_avoid_needs = GetStartRegionAvoidListForCiv_GetIDs(civType)
						iNumAvoidCivs = iNumAvoidCivs + 1;
						iNumAvoidCivsRemaining = iNumAvoidCivsRemaining + 1;
						table.insert(civs_needing_region_avoid, playerNum);
						avoid_lists[playerNum] = table_of_this_civs_avoid_needs;
					end
				end
			end
		end
	end
	
	print("Civs with Coastal Bias:", iNumCoastalCivs);
	print("Civs with River Bias:", iNumRiverCivs);
	print("Civs with Region Priority:", iNumPriorityCivs);
	print("Civs with Region Avoid:", iNumAvoidCivs); print("-");
	

	if iNumCoastalCivs > 0 then

		local iNumRegionsWithCoastalStart, iNumRegionsWithLakeStart, iNumUnassignableCoastStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][1] == true then
					print("Region#", region_number, "has a Coastal Start.");
					iNumRegionsWithCoastalStart = iNumRegionsWithCoastalStart + 1;
					table.insert(regions_with_coastal_start, region_number);
				end
			end
		end
		if iNumRegionsWithCoastalStart < iNumCoastalCivs then
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][2] == true and
					   self.startLocationConditions[region_number][1] == false then
						print("Region#", region_number, "has a Lake Start.");
						iNumRegionsWithLakeStart = iNumRegionsWithLakeStart + 1;
						table.insert(regions_with_lake_start, region_number);
					end
				end
			end
		end
		if iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart < iNumCoastalCivs then
			iNumUnassignableCoastStarts = iNumCoastalCivs - (iNumRegionsWithCoastalStart + iNumRegionsWithLakeStart);
		end

		print("iNumCoastalCivs: " .. iNumCoastalCivs);
		print("iNumUnassignableCoastStarts: " .. iNumUnassignableCoastStarts);
		if iNumCoastalCivs - iNumUnassignableCoastStarts > 0 then

			local non_priority_coastal_start = {};
			for loop1, iPlayerNum1 in ipairs(civs_needing_coastal_start) do
				local bAdd = true;
				for loop2, iPlayerNum2 in ipairs(civs_priority_coastal_start) do
					if (iPlayerNum1 == iPlayerNum2) then
						bAdd = false;
					end
				end
				if bAdd then
					table.insert(non_priority_coastal_start, iPlayerNum1);
				end
			end
			
			local shuffled_priority_coastal_start = GetShuffledCopyOfTable(civs_priority_coastal_start);
			local shuffled_non_priority_coastal_start = GetShuffledCopyOfTable(non_priority_coastal_start);
			local shuffled_coastal_civs = {};
			

			for loop, iPlayerNum in ipairs(shuffled_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end
			

			for loop, iPlayerNum in ipairs(shuffled_non_priority_coastal_start) do
				table.insert(shuffled_coastal_civs, iPlayerNum);
			end			
			
			for loop, iPlayerNum in ipairs(shuffled_coastal_civs) do
				print("shuffled_coastal_civs[" .. loop .. "]: " .. iPlayerNum);
			end
			
			local shuffled_coastal_regions, shuffled_lake_regions;
			local current_lake_index = 1;
			if iNumRegionsWithCoastalStart > 0 then
				shuffled_coastal_regions = GetShuffledCopyOfTable(regions_with_coastal_start);
			end
			if iNumRegionsWithLakeStart > 0 then
				shuffled_lake_regions = GetShuffledCopyOfTable(regions_with_lake_start);
			end
			for loop, playerNum in ipairs(shuffled_coastal_civs) do
				if loop > iNumCoastalCivs - iNumUnassignableCoastStarts then

					break
				end

				if loop <= iNumRegionsWithCoastalStart then

					local choose_this_region = shuffled_coastal_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else

					local choose_this_region = shuffled_lake_regions[current_lake_index];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					iNumCoastalCivsRemaining = iNumCoastalCivsRemaining - 1;
					local a, b, c = IdentifyTableIndex(civs_needing_coastal_start, playerNum)
					if a then
						table.remove(civs_needing_coastal_start, c[1]);
					end
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
					current_lake_index = current_lake_index + 1;
				end
			end


		end
	end
	

	if iNumRiverCivs > 0 or iNumCoastalCivsRemaining > 0 then

		local iNumRegionsWithRiverStart, iNumRegionsNearRiverStart, iNumUnassignableRiverStarts = 0, 0, 0;
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][3] == true then
					iNumRegionsWithRiverStart = iNumRegionsWithRiverStart + 1;
					table.insert(regions_with_river_start, region_number);
				end
			end
		end
		for region_number, bAlreadyAssigned in ipairs(region_status) do
			if bAlreadyAssigned == false then
				if self.startLocationConditions[region_number][4] == true and
				   self.startLocationConditions[region_number][3] == false then
					iNumRegionsNearRiverStart = iNumRegionsNearRiverStart + 1;
					table.insert(regions_with_near_river_start, region_number);
				end
			end
		end
		if iNumRegionsWithRiverStart + iNumRegionsNearRiverStart < iNumRiverCivs then
			iNumUnassignableRiverStarts = iNumRiverCivs - (iNumRegionsWithRiverStart + iNumRegionsNearRiverStart);
		end


		if iNumRiverCivs - iNumUnassignableRiverStarts > 0 then
			local shuffled_river_civs = GetShuffledCopyOfTable(civs_needing_river_start);
			local shuffled_river_regions, shuffled_near_river_regions;
			if iNumRegionsWithRiverStart > 0 then
				shuffled_river_regions = GetShuffledCopyOfTable(regions_with_river_start);
			end
			if iNumRegionsNearRiverStart > 0 then
				shuffled_near_river_regions = GetShuffledCopyOfTable(regions_with_near_river_start);
			end
			for loop, playerNum in ipairs(shuffled_river_civs) do
				if loop > iNumRiverCivs - iNumUnassignableRiverStarts then

					break
				end

				if loop <= iNumRegionsWithRiverStart then

					local choose_this_region = shuffled_river_regions[loop];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else

					local choose_this_region = shuffled_near_river_regions[loop - iNumRegionsWithRiverStart];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[playerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[playerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end

		if iNumCoastalCivsRemaining > 0 and iNumRiverCivs < iNumRegionsWithRiverStart + iNumRegionsNearRiverStart then
			local iNumFallbacksWithRiverStart, iNumFallbacksNearRiverStart = 0, 0;
			local fallbacks_with_river_start, fallbacks_with_near_river_start = {}, {};
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][3] == true then
						iNumFallbacksWithRiverStart = iNumFallbacksWithRiverStart + 1;
						table.insert(fallbacks_with_river_start, region_number);
					end
				end
			end
			for region_number, bAlreadyAssigned in ipairs(region_status) do
				if bAlreadyAssigned == false then
					if self.startLocationConditions[region_number][4] == true and
					   self.startLocationConditions[region_number][3] == false then
						iNumFallbacksNearRiverStart = iNumFallbacksNearRiverStart + 1;
						table.insert(fallbacks_with_near_river_start, region_number);
					end
				end
			end
			if iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart > 0 then
			
				local shuffled_coastal_fallback_civs = GetShuffledCopyOfTable(civs_needing_coastal_start);
				local shuffled_river_fallbacks, shuffled_near_river_fallbacks;
				if iNumFallbacksWithRiverStart > 0 then
					shuffled_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_river_start);
				end
				if iNumFallbacksNearRiverStart > 0 then
					shuffled_near_river_fallbacks = GetShuffledCopyOfTable(fallbacks_with_near_river_start);
				end
				for loop, playerNum in ipairs(shuffled_coastal_fallback_civs) do
					if loop > iNumFallbacksWithRiverStart + iNumFallbacksNearRiverStart then

						break
					end

					if loop <= iNumFallbacksWithRiverStart then

						local choose_this_region = shuffled_river_fallbacks[loop];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);

						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					else

						local choose_this_region = shuffled_near_river_fallbacks[loop - iNumRegionsWithRiverStart];
						local x = self.startingPlots[choose_this_region][1];
						local y = self.startingPlots[choose_this_region][2];
						local plot = Map.GetPlot(x, y);
						local player = Players[playerNum];
						player:SetStartingPlot(plot);

						region_status[choose_this_region] = true;
						civ_status[playerNum + 1] = true;
						local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
						if a then
							table.remove(regions_still_available, c[1]);
						end
					end
				end
			end
		end
	end
	

	if iNumPriorityCivs > 0 then
		print("-"); print("-"); print("--- REGION PRIORITY READOUT ---"); print("-");
		local iNumSinglePriority, iNumMultiPriority, iNumNeedFallbackPriority = 0, 0, 0;
		local single_priority, multi_priority, fallback_priority = {}, {}, {};
		local single_sorted, multi_sorted = {}, {};

		for playerNum, priority_needs in pairs(priority_lists) do
			local len = table.maxn(priority_needs)
			if len == 1 then
				print("Player#", playerNum, "has a single Region Priority of type", priority_needs[1]);
				local priority_data = {playerNum, priority_needs[1]};
				table.insert(single_priority, priority_data)
				iNumSinglePriority = iNumSinglePriority + 1;
			else
				print("Player#", playerNum, "has multiple Region Priority, this many types:", len);
				local priority_data = {playerNum, len};
				table.insert(multi_priority, priority_data)
				iNumMultiPriority = iNumMultiPriority + 1;
			end
		end

		if iNumSinglePriority > 0 then

			for region_type = 1, 9 do
				for loop, data in ipairs(single_priority) do
					if data[2] == region_type then

						table.insert(single_sorted, data);
					end
				end
			end

			for loop, data in ipairs(single_sorted) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				print("* Attempting to assign Player#", iPlayerNum, "to a region of Type#", iPriorityType);
				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					if self.regionTypes[region_number] == iPriorityType then
						table.insert(candidate_regions, region_number);
						bFoundCandidate = true;

					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);
					print("Player Number", iPlayerNum, "with single Region Priority assigned to Region#", choose_this_region, "at Plot", x, y);
					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				else
					table.insert(fallback_priority, data)
					iNumNeedFallbackPriority = iNumNeedFallbackPriority + 1;

				end
			end
		end

		if iNumMultiPriority > 0 then
			for iNumPriorities = 2, 8 do
				for loop, data in ipairs(multi_priority) do
					if data[2] == iNumPriorities then

						table.insert(multi_sorted, data);
					end
				end
			end

			for loop, data in ipairs(multi_sorted) do
				local iPlayerNum = data[1];
				local iNumPriorityTypes = data[2];

				local bFoundCandidate, candidate_regions = false, {};
				for test_loop, region_number in ipairs(regions_still_available) do
					for inner_loop = 1, iNumPriorityTypes do
						local region_type_to_test = priority_lists[iPlayerNum][inner_loop];
						if self.regionTypes[region_number] == region_type_to_test then
							table.insert(candidate_regions, region_number);
							bFoundCandidate = true;

						end
					end
				end
				if bFoundCandidate then
					local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
					local choose_this_region = candidate_regions[diceroll];
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end


				end
			end
		end

		if iNumNeedFallbackPriority > 0 then
			for loop, data in ipairs(fallback_priority) do
				local iPlayerNum = data[1];
				local iPriorityType = data[2];
				print("* Attempting to assign Player#", iPlayerNum, "to a fallback region as similar as possible to Region Type#", iPriorityType);
				local choose_this_region = self:FindFallbackForUnmatchedRegionPriority(iPriorityType, regions_still_available)
				if choose_this_region == -1 then

				else
					local x = self.startingPlots[choose_this_region][1];
					local y = self.startingPlots[choose_this_region][2];
					local plot = Map.GetPlot(x, y);
					local player = Players[iPlayerNum];
					player:SetStartingPlot(plot);

					region_status[choose_this_region] = true;
					civ_status[iPlayerNum + 1] = true;
					local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
					if a then
						table.remove(regions_still_available, c[1]);
					end
				end
			end
		end
	end
	

	if iNumAvoidCivs > 0 then

		local avoid_sorted, avoid_unsorted, avoid_counts = {}, {}, {};

		for playerNum, avoid_needs in pairs(avoid_lists) do
			local len = table.maxn(avoid_needs)

			local avoid_data = {playerNum, len};
			table.insert(avoid_unsorted, avoid_data)
			table.insert(avoid_counts, len)
		end
		table.sort(avoid_counts)
		for loop, avoid_count in ipairs(avoid_counts) do
			for test_loop, avoid_data in ipairs(avoid_unsorted) do
				if avoid_count == avoid_data[2] then
					table.insert(avoid_sorted, avoid_data[1])
					table.remove(avoid_unsorted, test_loop)
				end
			end
		end

		for loop = iNumAvoidCivs, 1, -1 do
			local iPlayerNum = avoid_sorted[loop];
			local candidate_regions = {};
			for test_loop, region_number in ipairs(regions_still_available) do
				local bFoundCandidate = true;
				for inner_loop, region_type_to_avoid in ipairs(avoid_lists[iPlayerNum]) do
					if self.regionTypes[region_number] == region_type_to_avoid then
						bFoundCandidate = false;
					end
				end
				if bFoundCandidate == true then
					table.insert(candidate_regions, region_number);

				end
			end
			if table.maxn(candidate_regions) > 0 then
				local diceroll = 1 + Map.Rand(table.maxn(candidate_regions), "Choosing from among Candidate Regions for start bias - LUA");
				local choose_this_region = candidate_regions[diceroll];
				local x = self.startingPlots[choose_this_region][1];
				local y = self.startingPlots[choose_this_region][2];
				local plot = Map.GetPlot(x, y);
				local player = Players[iPlayerNum];
				player:SetStartingPlot(plot);

				region_status[choose_this_region] = true;
				civ_status[iPlayerNum + 1] = true;
				local a, b, c = IdentifyTableIndex(regions_still_available, choose_this_region)
				if a then
					table.remove(regions_still_available, c[1]);
				end


			end
		end
	end
				

	local playerList, regionList = {}, {};
	for loop = 1, self.iNumCivs do
		local player_ID = self.player_ID_list[loop];
		if civ_status[player_ID + 1] == false then
			table.insert(playerList, player_ID);
		end
		if region_status[loop] == false then
			table.insert(regionList, loop);
		end
	end
	local iNumRemainingPlayers = table.maxn(playerList);
	local iNumRemainingRegions = table.maxn(regionList);
	if iNumRemainingPlayers > 0 or iNumRemainingRegions > 0 then




		if iNumRemainingPlayers ~= iNumRemainingRegions then
			print("-"); print("ERROR: Number of civs remaining after handling biases does not match number of regions remaining!"); print("-");
		end
		local playerListShuffled = GetShuffledCopyOfTable(playerList)
		for index, player_ID in ipairs(playerListShuffled) do
			local region_number = regionList[index];
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];

			local start_plot = Map.GetPlot(x, y)
			local player = Players[player_ID]
			player:SetStartingPlot(start_plot)
		end
	end




	if self.bTeamGame == true then
		self:NormalizeTeamLocations()
	end

end



function AssignStartingPlots:ExaminePlotForNaturalWondersEligibility(x, y)





	local iW, iH = Map.GetGridSize();
	local plotIndex = iW * y + x + 1;

	if self.naturalWondersData[plotIndex] > 0 then
		return false
	end
	return true
end

function AssignStartingPlots:ExamineCandidatePlotForNaturalWondersEligibility(x, y)



	if self:ExaminePlotForNaturalWondersEligibility(x, y) == false then
		return false
	end
	local iW, iH = Map.GetGridSize();



	for loop, direction in ipairs(self.direction_types) do
		local adjPlot = Map.PlotDirection(x, y, direction)
		if adjPlot == nil then
			return false
		else
			local adjX = adjPlot:GetX();
			local adjY = adjPlot:GetY();
			if self:ExaminePlotForNaturalWondersEligibility(adjX, adjY) == false then
				return false
			end
		end
	end
	return true
end

function AssignStartingPlots:CanBeThisNaturalWonderType(x, y, wn, rn)


	local plot = Map.GetPlot(x, y);

	if self.EligibilityMethodNumber[wn] ~= -1 then
		local method_number = self.EligibilityMethodNumber[wn];
		if NWCustomEligibility(x, y, method_number) == true then
			local iW, iH = Map.GetGridSize();
			local plotIndex = y * iW + x + 1;
			table.insert(self.eligibility_lists[wn], plotIndex);
		end
		return
	end

	if self.bWorldHasOceans == true then
		if self.RequireBiggestLandmass[wn] == true then
			local iAreaID = plot:GetArea();
			if iAreaID ~= self.iBiggestLandmassID then
				return
			end
		elseif self.AvoidBiggestLandmass[wn] == true then
			local iAreaID = plot:GetArea();
			if iAreaID == self.iBiggestLandmassID then
				return
			end
		end
	end
	if self.RequireFreshWater[wn] == true then
		if plot:IsFreshWater() == false then
			return
		end
	elseif self.AvoidFreshWater[wn] == true then
		if plot:IsRiver() or plot:IsLake() or plot:IsFreshWater() then
			return
		end
	end

	if self.LandBased[wn] == true then
		if plot:IsWater() == true then
			return
		end
		local iW, iH = Map.GetGridSize();
		local plotIndex = y * iW + x + 1;
		if self.RequireLandAdjacentToOcean[wn] == true then
			if self.plotDataIsCoastal[plotIndex] == false then
				return
			end
		elseif self.AvoidLandAdjacentToOcean[wn] == true then
			if self.plotDataIsCoastal[plotIndex] == true then
				return
			end
		end
		if self.RequireLandOnePlotInland[wn] == true then
			if self.plotDataIsNextToCoast[plotIndex] == false then
				return
			end
		elseif self.AvoidLandOnePlotInland[wn] == true then
			if self.plotDataIsNextToCoast[plotIndex] == true then
				return
			end
		end
		if self.RequireLandTwoOrMorePlotsInland[wn] == true then
			if self.plotDataIsCoastal[plotIndex] == true then
				return
			elseif self.plotDataIsNextToCoast[plotIndex] == true then
				return
			end
		elseif self.AvoidLandTwoOrMorePlotsInland[wn] == true then
			if self.plotDataIsCoastal[plotIndex] == false and self.plotDataIsNextToCoast[plotIndex] == false then
				return
			end
		end
	end

	if self.CoreTileCanBeAnyPlotType[wn] == false then
		local plotType = plot:GetPlotType()
		if plotType == PlotTypes.PLOT_LAND and self.CoreTileCanBeFlatland[wn] == true then

		elseif plotType == PlotTypes.PLOT_HILLS and self.CoreTileCanBeHills[wn] == true then

		elseif plotType == PlotTypes.PLOT_MOUNTAIN and self.CoreTileCanBeMountain[wn] == true then

		elseif plotType == PlotTypes.PLOT_OCEAN and self.CoreTileCanBeOcean[wn] == true then

		else
			return
		end
	end
	if self.CoreTileCanBeAnyTerrainType[wn] == false then
		local terrainType = plot:GetTerrainType()
		if terrainType == TerrainTypes.TERRAIN_GRASS and self.CoreTileCanBeGrass[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_PLAINS and self.CoreTileCanBePlains[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_DESERT and self.CoreTileCanBeDesert[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and self.CoreTileCanBeTundra[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_SNOW and self.CoreTileCanBeSnow[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_COAST and self.CoreTileCanBeShallowWater[wn] == true then

		elseif terrainType == TerrainTypes.TERRAIN_OCEAN and self.CoreTileCanBeDeepWater[wn] == true then

		else
			return
		end
	end
	if self.CoreTileCanBeAnyFeatureType[wn] == false then
		local featureType = plot:GetFeatureType()
		if featureType == FeatureTypes.NO_FEATURE and self.CoreTileCanBeNoFeature[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_FOREST and self.CoreTileCanBeForest[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_JUNGLE and self.CoreTileCanBeJungle[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_OASIS and self.CoreTileCanBeOasis[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS and self.CoreTileCanBeFloodPlains[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_MARSH and self.CoreTileCanBeMarsh[wn] == true then

		elseif featureType == FeatureTypes.FEATURE_ICE and self.CoreTileCanBeIce[wn] == true then

		elseif featureType == self.feature_atoll and self.CoreTileCanBeAtoll[wn] == true then

		else
			return
		end
	end

	if self.AdjacentTilesCareAboutPlotTypes[wn] == true then
		local iNumAnyLand, iNumFlatland, iNumHills, iNumMountain, iNumHillsPlusMountains, iNumOcean = 0, 0, 0, 0, 0, 0;
		for loop, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			local plotType = adjPlot:GetPlotType();
			if plotType == PlotTypes.PLOT_OCEAN then
				iNumOcean = iNumOcean + 1;
			else
				iNumAnyLand = iNumAnyLand + 1;
				if plotType == PlotTypes.PLOT_LAND then
					iNumFlatland = iNumFlatland + 1;
				else
					iNumHillsPlusMountains = iNumHillsPlusMountains + 1;
					if plotType == PlotTypes.PLOT_HILLS then
						iNumHills = iNumHills + 1;
					else
						iNumMountain = iNumMountain + 1;
					end
				end
			end
		end
		if iNumAnyLand > 0 and self.AdjacentTilesAvoidAnyland[wn] == true then
			return
		end

		if self.AdjacentTilesRequireFlatland[wn] == true then
			if iNumFlatland < self.RequiredNumberOfAdjacentFlatland[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireHills[wn] == true then
			if iNumHills < self.RequiredNumberOfAdjacentHills[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireMountain[wn] == true then
			if iNumMountain < self.RequiredNumberOfAdjacentMountain[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireHillsPlusMountains[wn] == true then
			if iNumHillsPlusMountains < self.RequiredNumberOfAdjacentHillsPlusMountains[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireOcean[wn] == true then
			if iNumOcean < self.RequiredNumberOfAdjacentOcean[wn] then
				return
			end
		end

		if self.AdjacentTilesAvoidFlatland[wn] == true then
			if iNumFlatland > self.MaximumAllowedAdjacentFlatland[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidHills[wn] == true then
			if iNumHills > self.MaximumAllowedAdjacentHills[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidMountain[wn] == true then
			if iNumMountain > self.MaximumAllowedAdjacentMountain[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidHillsPlusMountains[wn] == true then
			if iNumHillsPlusMountains > self.MaximumAllowedAdjacentHillsPlusMountains[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidOcean[wn] == true then
			if iNumOcean > self.MaximumAllowedAdjacentOcean[wn] then
				return
			end
		end
	end

	if self.AdjacentTilesCareAboutTerrainTypes[wn] == true then
		local iNumGrass, iNumPlains, iNumDesert, iNumTundra, iNumSnow, iNumShallowWater, iNumDeepWater = 0, 0, 0, 0, 0, 0, 0;
		for loop, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			local terrainType = adjPlot:GetTerrainType();
			if terrainType == TerrainTypes.TERRAIN_GRASS then
				iNumGrass = iNumGrass + 1;
			elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
				iNumPlains = iNumPlains + 1;
			elseif terrainType == TerrainTypes.TERRAIN_DESERT then
				iNumDesert = iNumDesert + 1;
			elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
				iNumTundra = iNumTundra + 1;
			elseif terrainType == TerrainTypes.TERRAIN_SNOW then
				iNumSnow = iNumSnow + 1;
			elseif terrainType == TerrainTypes.TERRAIN_COAST then
				iNumShallowWater = iNumShallowWater + 1;
			elseif terrainType == TerrainTypes.TERRAIN_OCEAN then
				iNumDeepWater = iNumDeepWater + 1;
			end
		end

		if self.AdjacentTilesRequireGrass[wn] == true then
			if iNumGrass < self.RequiredNumberOfAdjacentGrass[wn] then
				return
			end
		end
		if self.AdjacentTilesRequirePlains[wn] == true then
			if iNumPlains < self.RequiredNumberOfAdjacentPlains[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireDesert[wn] == true then
			if iNumDesert < self.RequiredNumberOfAdjacentDesert[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireTundra[wn] == true then
			if iNumTundra < self.RequiredNumberOfAdjacentTundra[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireSnow[wn] == true then
			if iNumSnow < self.RequiredNumberOfAdjacentSnow[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireShallowWater[wn] == true then
			if iNumShallowWater < self.RequiredNumberOfAdjacentShallowWater[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireGrass[wn] == true then
			if iNumDeepWater < self.RequiredNumberOfAdjacentDeepWater[wn] then
				return
			end
		end

		if self.AdjacentTilesAvoidGrass[wn] == true then
			if iNumGrass > self.MaximumAllowedAdjacentGrass[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidPlains[wn] == true then
			if iNumPlains > self.MaximumAllowedAdjacentPlains[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidDesert[wn] == true then
			if iNumDesert > self.MaximumAllowedAdjacentDesert[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidTundra[wn] == true then
			if iNumTundra > self.MaximumAllowedAdjacentTundra[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidSnow[wn] == true then
			if iNumSnow > self.MaximumAllowedAdjacentSnow[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidShallowWater[wn] == true then
			if iNumShallowWater > self.MaximumAllowedAdjacentShallowWater[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidDeepWater[wn] == true then
			if iNumDeepWater > self.MaximumAllowedAdjacentDeepWater[wn] then
				return
			end
		end
	end

	if self.AdjacentTilesCareAboutFeatureTypes[wn] == true then
		local iNumNoFeature, iNumForest, iNumJungle, iNumOasis, iNumFloodPlains, iNumMarsh, iNumIce, iNumAtoll = 0, 0, 0, 0, 0, 0, 0, 0;
		for loop, direction in ipairs(self.direction_types) do
			local adjPlot = Map.PlotDirection(x, y, direction)
			local featureType = adjPlot:GetFeatureType();
			if featureType == FeatureTypes.NO_FEATURE then
				iNumNoFeature = iNumNoFeature + 1;
			elseif featureType == FeatureTypes.FEATURE_FOREST then
				iNumForest = iNumForest + 1;
			elseif featureType == FeatureTypes.FEATURE_JUNGLE then
				iNumJungle = iNumJungle + 1;
			elseif featureType == FeatureTypes.FEATURE_OASIS then
				iNumOasis = iNumOasis + 1;
			elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
				iNumFloodPlains = iNumFloodPlains + 1;
			elseif featureType == FeatureTypes.FEATURE_MARSH then
				iNumMarsh = iNumMarsh + 1;
			elseif featureType == FeatureTypes.FEATURE_ICE then
				iNumIce = iNumIce + 1;
			elseif featureType == self.feature_atoll then
				iNumAtoll = iNumAtoll + 1;
			end
		end

		if self.AdjacentTilesRequireNoFeature[wn] == true then
			if iNumNoFeature < self.RequiredNumberOfAdjacentNoFeature[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireForest[wn] == true then
			if iNumForest < self.RequiredNumberOfAdjacentForest[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireJungle[wn] == true then
			if iNumJungle < self.RequiredNumberOfAdjacentJungle[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireOasis[wn] == true then
			if iNumOasis < self.RequiredNumberOfAdjacentOasis[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireFloodPlains[wn] == true then
			if iNumFloodPlains < self.RequiredNumberOfAdjacentFloodPlains[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireMarsh[wn] == true then
			if iNumMarsh < self.RequiredNumberOfAdjacentMarsh[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireIce[wn] == true then
			if iNumIce < self.RequiredNumberOfAdjacentIce[wn] then
				return
			end
		end
		if self.AdjacentTilesRequireAtoll[wn] == true then
			if iNumAtoll < self.RequiredNumberOfAdjacentAtoll[wn] then
				return
			end
		end

		if self.AdjacentTilesAvoidNoFeature[wn] == true then
			if iNumNoFeature > self.MaximumAllowedAdjacentNoFeature[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidForest[wn] == true then
			if iNumForest > self.MaximumAllowedAdjacentForest[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidJungle[wn] == true then
			if iNumJungle > self.MaximumAllowedAdjacentJungle[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidOasis[wn] == true then
			if iNumOasis > self.MaximumAllowedAdjacentOasis[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidFloodPlains[wn] == true then
			if iNumFloodPlains > self.MaximumAllowedAdjacentFloodPlains[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidMarsh[wn] == true then
			if iNumMarsh > self.MaximumAllowedAdjacentMarsh[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidIce[wn] == true then
			if iNumIce > self.MaximumAllowedAdjacentIce[wn] then
				return
			end
		end
		if self.AdjacentTilesAvoidAtoll[wn] == true then
			if iNumAtoll > self.MaximumAllowedAdjacentAtoll[wn] then
				return
			end
		end
	end


	local iW, iH = Map.GetGridSize();
	local plotIndex = y * iW + x + 1;
	table.insert(self.eligibility_lists[wn], plotIndex);
end

function AssignStartingPlots:GenerateLocalVersionsOfDataFromXML()
	for nw_number, rn in ipairs(self.xml_row_numbers) do
		table.insert(self.EligibilityMethodNumber, GameInfo.Natural_Wonder_Placement[rn].EligibilityMethodNumber);
		table.insert(self.OccurrenceFrequency, GameInfo.Natural_Wonder_Placement[rn].OccurrenceFrequency);		
		table.insert(self.RequireBiggestLandmass, GameInfo.Natural_Wonder_Placement[rn].RequireBiggestLandmass);

		table.insert(self.AvoidBiggestLandmass, false);
		table.insert(self.RequireFreshWater, GameInfo.Natural_Wonder_Placement[rn].RequireFreshWater);
		table.insert(self.AvoidFreshWater, GameInfo.Natural_Wonder_Placement[rn].AvoidFreshWater);
		table.insert(self.LandBased, GameInfo.Natural_Wonder_Placement[rn].LandBased);
		table.insert(self.RequireLandAdjacentToOcean, GameInfo.Natural_Wonder_Placement[rn].RequireLandAdjacentToOcean);
		table.insert(self.AvoidLandAdjacentToOcean, GameInfo.Natural_Wonder_Placement[rn].AvoidLandAdjacentToOcean);
		table.insert(self.RequireLandOnePlotInland, GameInfo.Natural_Wonder_Placement[rn].RequireLandOnePlotInland);
		table.insert(self.AvoidLandOnePlotInland, GameInfo.Natural_Wonder_Placement[rn].AvoidLandOnePlotInland);
		table.insert(self.RequireLandTwoOrMorePlotsInland, GameInfo.Natural_Wonder_Placement[rn].RequireLandTwoOrMorePlotsInland);
		table.insert(self.AvoidLandTwoOrMorePlotsInland, GameInfo.Natural_Wonder_Placement[rn].AvoidLandTwoOrMorePlotsInland);

		table.insert(self.CoreTileCanBeAnyPlotType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyPlotType);
		table.insert(self.CoreTileCanBeFlatland, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeFlatland);
		table.insert(self.CoreTileCanBeHills, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeHills);
		table.insert(self.CoreTileCanBeMountain, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeMountain);
		table.insert(self.CoreTileCanBeOcean, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeOcean);
		table.insert(self.CoreTileCanBeAnyTerrainType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyTerrainType);
		table.insert(self.CoreTileCanBeGrass, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeGrass);
		table.insert(self.CoreTileCanBePlains, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBePlains);
		table.insert(self.CoreTileCanBeDesert, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeDesert);
		table.insert(self.CoreTileCanBeTundra, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeTundra);
		table.insert(self.CoreTileCanBeSnow, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeSnow);
		table.insert(self.CoreTileCanBeShallowWater, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeShallowWater);
		table.insert(self.CoreTileCanBeDeepWater, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeDeepWater);
		table.insert(self.CoreTileCanBeAnyFeatureType, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAnyFeatureType);
		table.insert(self.CoreTileCanBeNoFeature, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeNoFeature);
		table.insert(self.CoreTileCanBeForest, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeForest);
		table.insert(self.CoreTileCanBeJungle, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeJungle);
		table.insert(self.CoreTileCanBeOasis, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeOasis);
		table.insert(self.CoreTileCanBeFloodPlains, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeFloodPlains);
		table.insert(self.CoreTileCanBeMarsh, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeMarsh);
		table.insert(self.CoreTileCanBeIce, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeIce);
		table.insert(self.CoreTileCanBeAtoll, GameInfo.Natural_Wonder_Placement[rn].CoreTileCanBeAtoll);

		table.insert(self.AdjacentTilesCareAboutPlotTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutPlotTypes);
		table.insert(self.AdjacentTilesAvoidAnyland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidAnyland);
		table.insert(self.AdjacentTilesRequireFlatland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireFlatland);
		table.insert(self.RequiredNumberOfAdjacentFlatland, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentFlatland);
		table.insert(self.AdjacentTilesRequireHills, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireHills);
		table.insert(self.RequiredNumberOfAdjacentHills, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentHills);
		table.insert(self.AdjacentTilesRequireMountain, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireMountain);
		table.insert(self.RequiredNumberOfAdjacentMountain, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentMountain);
		table.insert(self.AdjacentTilesRequireHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireHillsPlusMountains);
		table.insert(self.RequiredNumberOfAdjacentHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentHillsPlusMountains);
		table.insert(self.AdjacentTilesRequireOcean, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireOcean);
		table.insert(self.RequiredNumberOfAdjacentOcean, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentOcean);
		table.insert(self.AdjacentTilesAvoidFlatland, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidFlatland);
		table.insert(self.MaximumAllowedAdjacentFlatland, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentFlatland);
		table.insert(self.AdjacentTilesAvoidHills, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidHills);
		table.insert(self.MaximumAllowedAdjacentHills, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentHills);
		table.insert(self.AdjacentTilesAvoidMountain, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidMountain);
		table.insert(self.MaximumAllowedAdjacentMountain, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentMountain);
		table.insert(self.AdjacentTilesAvoidHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidHillsPlusMountains);
		table.insert(self.MaximumAllowedAdjacentHillsPlusMountains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentHillsPlusMountains);
		table.insert(self.AdjacentTilesAvoidOcean, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidOcean);
		table.insert(self.MaximumAllowedAdjacentOcean, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentOcean);

		table.insert(self.AdjacentTilesCareAboutTerrainTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutTerrainTypes);
		table.insert(self.AdjacentTilesRequireGrass, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireGrass);
		table.insert(self.RequiredNumberOfAdjacentGrass, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentGrass);
		table.insert(self.AdjacentTilesRequirePlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequirePlains);
		table.insert(self.RequiredNumberOfAdjacentPlains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentPlains);
		table.insert(self.AdjacentTilesRequireDesert, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireDesert);
		table.insert(self.RequiredNumberOfAdjacentDesert, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentDesert);
		table.insert(self.AdjacentTilesRequireTundra, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireTundra);
		table.insert(self.RequiredNumberOfAdjacentTundra, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentTundra);
		table.insert(self.AdjacentTilesRequireSnow, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireSnow);
		table.insert(self.RequiredNumberOfAdjacentSnow, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentSnow);
		table.insert(self.AdjacentTilesRequireShallowWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireShallowWater);
		table.insert(self.RequiredNumberOfAdjacentShallowWater, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentShallowWater);
		table.insert(self.AdjacentTilesRequireDeepWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireDeepWater);
		table.insert(self.RequiredNumberOfAdjacentDeepWater, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentDeepWater);
		table.insert(self.AdjacentTilesAvoidGrass, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidGrass);
		table.insert(self.MaximumAllowedAdjacentGrass, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentGrass);
		table.insert(self.AdjacentTilesAvoidPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidPlains);
		table.insert(self.MaximumAllowedAdjacentPlains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentPlains);
		table.insert(self.AdjacentTilesAvoidDesert, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidDesert);
		table.insert(self.MaximumAllowedAdjacentDesert, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentDesert);
		table.insert(self.AdjacentTilesAvoidTundra, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidTundra);
		table.insert(self.MaximumAllowedAdjacentTundra, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentTundra);
		table.insert(self.AdjacentTilesAvoidSnow, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidSnow);
		table.insert(self.MaximumAllowedAdjacentSnow, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentSnow);
		table.insert(self.AdjacentTilesAvoidShallowWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidShallowWater);
		table.insert(self.MaximumAllowedAdjacentShallowWater, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentShallowWater);
		table.insert(self.AdjacentTilesAvoidDeepWater, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidDeepWater);
		table.insert(self.MaximumAllowedAdjacentDeepWater, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentDeepWater);
		
		table.insert(self.AdjacentTilesCareAboutFeatureTypes, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesCareAboutFeatureTypes);
		table.insert(self.AdjacentTilesRequireNoFeature, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireNoFeature);
		table.insert(self.RequiredNumberOfAdjacentNoFeature, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentNoFeature);
		table.insert(self.AdjacentTilesRequireForest, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireForest);
		table.insert(self.RequiredNumberOfAdjacentForest, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentForest);
		table.insert(self.AdjacentTilesRequireJungle, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireJungle);
		table.insert(self.RequiredNumberOfAdjacentJungle, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentJungle);
		table.insert(self.AdjacentTilesRequireOasis, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireOasis);
		table.insert(self.RequiredNumberOfAdjacentOasis, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentOasis);
		table.insert(self.AdjacentTilesRequireFloodPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireFloodPlains);
		table.insert(self.RequiredNumberOfAdjacentFloodPlains, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentFloodPlains);
		table.insert(self.AdjacentTilesRequireMarsh, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireMarsh);
		table.insert(self.RequiredNumberOfAdjacentMarsh, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentMarsh);
		table.insert(self.AdjacentTilesRequireIce, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireIce);
		table.insert(self.RequiredNumberOfAdjacentIce, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentIce);
		table.insert(self.AdjacentTilesRequireAtoll, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesRequireAtoll);
		table.insert(self.RequiredNumberOfAdjacentAtoll, GameInfo.Natural_Wonder_Placement[rn].RequiredNumberOfAdjacentAtoll);
		table.insert(self.AdjacentTilesAvoidNoFeature, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidNoFeature);
		table.insert(self.MaximumAllowedAdjacentNoFeature, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentNoFeature);
		table.insert(self.AdjacentTilesAvoidForest, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidForest);
		table.insert(self.MaximumAllowedAdjacentForest, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentForest);
		table.insert(self.AdjacentTilesAvoidJungle, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidJungle);
		table.insert(self.MaximumAllowedAdjacentJungle, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentJungle);
		table.insert(self.AdjacentTilesAvoidOasis, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidOasis);
		table.insert(self.MaximumAllowedAdjacentOasis, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentOasis);
		table.insert(self.AdjacentTilesAvoidFloodPlains, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidFloodPlains);
		table.insert(self.MaximumAllowedAdjacentFloodPlains, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentFloodPlains);
		table.insert(self.AdjacentTilesAvoidMarsh, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidMarsh);
		table.insert(self.MaximumAllowedAdjacentMarsh, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentMarsh);
		table.insert(self.AdjacentTilesAvoidIce, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidIce);
		table.insert(self.MaximumAllowedAdjacentIce, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentIce);
		table.insert(self.AdjacentTilesAvoidAtoll, GameInfo.Natural_Wonder_Placement[rn].AdjacentTilesAvoidAtoll);
		table.insert(self.MaximumAllowedAdjacentAtoll, GameInfo.Natural_Wonder_Placement[rn].MaximumAllowedAdjacentAtoll);
		
		table.insert(self.TileChangesMethodNumber, GameInfo.Natural_Wonder_Placement[rn].TileChangesMethodNumber);
		table.insert(self.ChangeCoreTileToMountain, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileToMountain);
		table.insert(self.ChangeCoreTileToFlatland, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileToFlatland);
		table.insert(self.ChangeCoreTileTerrainToGrass, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileTerrainToGrass);
		table.insert(self.ChangeCoreTileTerrainToPlains, GameInfo.Natural_Wonder_Placement[rn].ChangeCoreTileTerrainToPlains);
		table.insert(self.SetAdjacentTilesToShallowWater, GameInfo.Natural_Wonder_Placement[rn].SetAdjacentTilesToShallowWater);
	end
end

function AssignStartingPlots:GenerateNaturalWondersCandidatePlotLists()

	local iW, iH = Map.GetGridSize();

	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == "FEATURE_ATOLL" then
			self.feature_atoll = thisFeature.ID;
		end
	end

	local biggest_landmass = Map.FindBiggestArea(false)
	self.iBiggestLandmassID = biggest_landmass:GetID()
	local biggest_ocean = Map.FindBiggestArea(true)
	local iNumBiggestOceanPlots = 0;
	if biggest_ocean ~= nil then
		iNumBiggestOceanPlots = biggest_ocean:GetNumTiles()
	end
	if iNumBiggestOceanPlots > (iW * iH) / 4 then
		self.bWorldHasOceans = true;
	else
		self.bWorldHasOceans = false;
	end

	for row in GameInfo.Natural_Wonder_Placement() do
		self.iNumNW = self.iNumNW + 1;
	end
	if self.iNumNW == 0 then
		print("-"); print("*** No Natural Wonders found in Civ5Features.xml! ***"); print("-");
		return
	end

	self.wonder_list = table.fill(-1, self.iNumNW);
	local next_wonder_number = 1;
	for row in GameInfo.Features() do
		if (row.NaturalWonder == true) then
			self.wonder_list[next_wonder_number] = row.Type;
			next_wonder_number = next_wonder_number + 1;
		end
	end

	for i = 1, self.iNumNW do
		table.insert(self.eligibility_lists, {});
	end

	for nw_number, nw_type in ipairs(self.wonder_list) do

		local row_number;
		for row in GameInfo.Natural_Wonder_Placement() do
			if row.NaturalWonderType == nw_type then
				row_number = row.ID;
			end
		end
		table.insert(self.xml_row_numbers, row_number);
	end

	self:GenerateLocalVersionsOfDataFromXML()

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			if self:ExamineCandidatePlotForNaturalWondersEligibility(x, y) == true then

				for nw_number, row_number in ipairs(self.xml_row_numbers) do
					self:CanBeThisNaturalWonderType(x, y, nw_number, row_number)
				end
			end
		end
	end

	local iCanBeWonder = {};
	for loop = 1, self.iNumNW do
		table.insert(iCanBeWonder, table.maxn(self.eligibility_lists[loop]));

	end

	local NW_eligibility_order, NW_eligibility_unsorted, NW_eligibility_sorted, NW_remaining_to_sort_by_occurrence = {}, {}, {}, {}; 
	for loop = 1, self.iNumNW do
		if iCanBeWonder[loop] > 0 then
			table.insert(NW_eligibility_unsorted, {loop, iCanBeWonder[loop]});
			table.insert(NW_eligibility_sorted, iCanBeWonder[loop]);
		end
	end
	table.sort(NW_eligibility_sorted);
	

	for NW_order = 1, self.iNumNW do
		for loop, data_pair in ipairs(NW_eligibility_unsorted) do
			local unsorted_count = data_pair[2];
			if NW_eligibility_sorted[NW_order] == unsorted_count then
				local unsorted_NW_num = data_pair[1];
				table.insert(NW_eligibility_order, unsorted_NW_num);
				table.insert(NW_remaining_to_sort_by_occurrence, unsorted_NW_num);
				table.remove(NW_eligibility_unsorted, loop);
				break
			end
		end
	end
	

	print("-"); print("-"); print("--- Number of Candidate Plots on the map for Natural Wonders ---"); print("-");
	for loop = 1, self.iNumNW do
		print("-", iCanBeWonder[loop], "candidates for", self.wonder_list[loop]);
	end
	print("-"); print("--- End of candidates readout for Natural Wonders ---"); print("-");	





	local NW_candidate_pool_entries, NW_final_selections = {}, {};
	for loop, iNaturalWonderNumber in ipairs(NW_eligibility_order) do
		local nw_type = self.wonder_list[iNaturalWonderNumber];
		local row_number;
		for row in GameInfo.Natural_Wonder_Placement() do
			if row.NaturalWonderType == nw_type then
				row_number = row.ID;
			end
		end
		local iFrequency = GameInfo.Natural_Wonder_Placement[row_number].OccurrenceFrequency;



		for entry = 1, iFrequency do
			table.insert(NW_candidate_pool_entries, iNaturalWonderNumber);
		end
	end

	local iNumNWtoProcess = table.maxn(NW_remaining_to_sort_by_occurrence)
	if iNumNWtoProcess > 0 then

		local entry_count = table.maxn(NW_candidate_pool_entries)
		for loop = 1, iNumNWtoProcess do
			local current_NW_selected = false;
			local current_attempt_to_select = 0;
			while current_NW_selected == false do
				if current_attempt_to_select > 1000 then
					break
				end
				current_attempt_to_select = current_attempt_to_select + 1;

				local diceroll = 1 + Map.Rand(entry_count, "Checking a random pool entry for NW assignment - Lua");
				local possible_selection = NW_candidate_pool_entries[diceroll];
				local bFoundValue, iNumTimesFoundValue, table_of_indices = IdentifyTableIndex(NW_remaining_to_sort_by_occurrence, possible_selection)
				if bFoundValue then
					table.insert(NW_final_selections, possible_selection)
					table.remove(NW_remaining_to_sort_by_occurrence, table_of_indices[1])

					current_NW_selected = true;
				end
			end
		end
	end
	
	if NW_final_selections ~= nil then
		return NW_final_selections;
	else
		print("ERROR: Failed to produce final selection list of NWs!");
	end
end

function AssignStartingPlots:AttemptToPlaceNaturalWonder(wonder_number, row_number)

	local iW, iH = Map.GetGridSize();
	local feature_type_to_place;
	for thisFeature in GameInfo.Features() do
		if thisFeature.Type == self.wonder_list[wonder_number] then
			feature_type_to_place = thisFeature.ID;
			break
		end
	end
	local temp_table = self.eligibility_lists[wonder_number];
	local candidate_plot_list = GetShuffledCopyOfTable(temp_table)
	for loop, plotIndex in ipairs(candidate_plot_list) do
		if self.naturalWondersData[plotIndex] == 0 then
			local x = (plotIndex - 1) % iW;
			local y = (plotIndex - x - 1) / iW;
			local plot = Map.GetPlot(x, y);

			local method_number = GameInfo.Natural_Wonder_Placement[row_number].TileChangesMethodNumber;
			if method_number ~= -1 then

				NWCustomPlacement(x, y, row_number, method_number)
			else

				if GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileToMountain == true then
					if not plot:IsMountain() then
						plot:SetPlotType(PlotTypes.PLOT_MOUNTAIN, false, false);
					end
				elseif GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileToFlatland == true then
					if plot:GetPlotType() ~= PlotTypes.PLOT_LAND then
						plot:SetPlotType(PlotTypes.PLOT_LAND, false, false);
					end
				end
				if GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileTerrainToGrass == true then
					if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_GRASS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_GRASS, false, false);
					end
				elseif GameInfo.Natural_Wonder_Placement[row_number].ChangeCoreTileTerrainToPlains == true then
					if plot:GetTerrainType() ~= TerrainTypes.TERRAIN_PLAINS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, false);
					end
				end
				if GameInfo.Natural_Wonder_Placement[row_number].SetAdjacentTilesToShallowWater == true then
					for loop, direction in ipairs(self.direction_types) do
						local adjPlot = Map.PlotDirection(x, y, direction)
						if adjPlot:GetTerrainType() ~= TerrainTypes.TERRAIN_COAST then
							adjPlot:SetTerrainType(TerrainTypes.TERRAIN_COAST, false, false)
						end
					end
				end
			end

			plot:SetFeatureType(feature_type_to_place)
			table.insert(self.placed_natural_wonder, wonder_number);
			self:PlaceResourceImpact(x, y, 6, math.floor(iH / 5))
			self:PlaceResourceImpact(x, y, 1, 1)
			self:PlaceResourceImpact(x, y, 2, 1)
			self:PlaceResourceImpact(x, y, 3, 1)
			self:PlaceResourceImpact(x, y, 5, 2)
			self:PlaceResourceImpact(x, y, 7, 1)
			local plotIndex = y * iW + x + 1;
			self.playerCollisionData[plotIndex] = true;



			if (self.wonder_list[wonder_number] == "FEATURE_REEF") then

				local SEPlot = Map.PlotDirection(x, y, DirectionTypes.DIRECTION_SOUTHEAST)
				local southeastX = SEPlot:GetX()
				local southeastY = SEPlot:GetY()
				self:PlaceResourceImpact(southeastX, southeastY, 1, 1)
				self:PlaceResourceImpact(southeastX, southeastY, 2, 1)
				self:PlaceResourceImpact(southeastX, southeastY, 3, 1)
				local SEplotIndex = southeastY * iW + southeastX + 1
				self.playerCollisionData[SEplotIndex] = true
			end





			return true
		end
	end

	return false
end

function AssignStartingPlots:PlaceNaturalWonders(wonderargs)
	local NW_eligibility_order = self:GenerateNaturalWondersCandidatePlotLists()
	local iNumNWCandidates = table.maxn(NW_eligibility_order);
	if iNumNWCandidates == 0 then
		print("No Natural Wonders placed, no eligible sites found for any of them.");
		return
	end
	






	

	local wonderargs = wonderargs or 15;



	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 2,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 5,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 6,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 7
		}

	local target_number = worldsizes[Map.GetWorldSize()];
	
	print("######################");
	print("WonderArgs: ", wonderargs.wonderamt);
	print("######################");

	if wonderargs.wonderamt ~= 14 then
		target_number = wonderargs.wonderamt;
	end

	local iNumNWtoPlace = math.min(target_number, iNumNWCandidates);
	local selected_NWs, fallback_NWs = {}, {};
	for loop, NW in ipairs(NW_eligibility_order) do
		if loop <= iNumNWtoPlace then
			table.insert(selected_NWs, NW);
		else
			table.insert(fallback_NWs, NW);
		end
	end
	













	

	local iNumPlaced = 0;
	for loop, nw_number in ipairs(selected_NWs) do
		local nw_type = self.wonder_list[nw_number];

		local row_number;
		for row in GameInfo.Natural_Wonder_Placement() do
			if row.NaturalWonderType == nw_type then
				row_number = row.ID;
			end
		end

		local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number)
		if bSuccess then
			iNumPlaced = iNumPlaced + 1;
		end
	end
	if iNumPlaced < iNumNWtoPlace then
		for loop, nw_number in ipairs(fallback_NWs) do
			if iNumPlaced >= iNumNWtoPlace then
				break
			end
			local nw_type = self.wonder_list[nw_number];

			local row_number;
			for row in GameInfo.Natural_Wonder_Placement() do
				if row.NaturalWonderType == nw_type then
					row_number = row.ID;
				end
			end

			local bSuccess = self:AttemptToPlaceNaturalWonder(nw_number, row_number)
			if bSuccess then
				iNumPlaced = iNumPlaced + 1;
			end
		end
	end
	

	if iNumPlaced >= iNumNWtoPlace then
		print("-- Placed all Natural Wonders --"); print("-"); print("-");
	else
		print("-- Not all Natural Wonders targeted got placed --"); print("-"); print("-");
	end

		
end



function AssignStartingPlots:AssignCityStatesToRegionsOrToUninhabited(args)







	local iW, iH = Map.GetGridSize()
	local ratio = self.iNumCityStates / self.iNumCivs;
	if ratio > 14 then
		self.iNumCityStatesPerRegion = 10;
	elseif ratio > 11 then
		self.iNumCityStatesPerRegion = 8;
	elseif ratio > 8 then
		self.iNumCityStatesPerRegion = 7;
	elseif ratio > 5.7 then
		self.iNumCityStatesPerRegion = 5;
	elseif ratio > 4.35 then
		self.iNumCityStatesPerRegion = 4;
	elseif ratio > 2.7 then
		self.iNumCityStatesPerRegion = 3;
	elseif ratio > 1.35 then
		self.iNumCityStatesPerRegion = 2;
	else
		self.iNumCityStatesPerRegion = 0;
	end


	local current_cs_index = 1;

	if self.iNumCityStatesPerRegion > 0 then
		for current_region = 1, self.iNumCivs do
			for cs_to_assign_to_this_region = 1, self.iNumCityStatesPerRegion do
				self.city_state_region_assignments[current_cs_index] = current_region;

				current_cs_index = current_cs_index + 1;
				self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
			end
		end
	end



	local iNumLandAreas = 0;
	local iNumCivLandmassPlots = 0;
	local iNumUninhabitedLandmassPlots = 0;
	local land_area_IDs = {};
	local land_area_plot_count = {};
	local land_area_plot_tables = {};
	local areas_inhabited_by_civs = {};
	local areas_too_small = {};
	local areas_uninhabited = {};

	if self.method == 3 then
		self.iNumCityStatesUninhabited = 0;

	else

		if self.method == 1 or self.method == 2 then
			for index, region_data in ipairs(self.regionData) do
				local region_areaID = region_data[5];
				if TestMembership(areas_inhabited_by_civs, region_areaID) == false then
					table.insert(areas_inhabited_by_civs, region_areaID);
				end
			end
		end

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local plotIndex = y * iW + x + 1;
				local plot = Map.GetPlot(x, y);
				local plotType = plot:GetPlotType()
				local terrainType = plot:GetTerrainType()
				if (plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS) and terrainType ~= TerrainTypes.TERRAIN_SNOW then
					local iArea = plot:GetArea();
					if self.method == 4 then
						if (x >= self.inhabited_WestX and x <= self.inhabited_WestX + self.inhabited_Width - 1) and
						   (y >= self.inhabited_SouthY and y <= self.inhabited_SouthY + self.inhabited_Height - 1) then
							iNumCivLandmassPlots = iNumCivLandmassPlots + 1;
						else
							iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + 1;
							if self.plotDataIsCoastal[i] == true then
								table.insert(self.uninhabited_areas_coastal_plots, i);
							else
								table.insert(self.uninhabited_areas_inland_plots, i);
							end
						end
					else
						if TestMembership(land_area_IDs, iArea) == false then
							iNumLandAreas = iNumLandAreas + 1;
							table.insert(land_area_IDs, iArea);
							land_area_plot_count[iArea] = 1;
							land_area_plot_tables[iArea] = {plotIndex};
						else
							land_area_plot_count[iArea] = land_area_plot_count[iArea] + 1;
							table.insert(land_area_plot_tables[iArea], plotIndex);
						end
					end
				end
			end
		end

		if self.method == 1 or self.method == 2 then

			for areaID, plot_count in pairs(land_area_plot_count) do
				if TestMembership(areas_inhabited_by_civs, areaID) == true then 
					iNumCivLandmassPlots = iNumCivLandmassPlots + plot_count;
				else
					iNumUninhabitedLandmassPlots = iNumUninhabitedLandmassPlots + plot_count;
					if plot_count < 60 then
						table.insert(areas_too_small, areaID);
					else
						table.insert(areas_uninhabited, areaID);
					end
				end
			end

			for areaID, area_plot_list in pairs(land_area_plot_tables) do
				if TestMembership(areas_uninhabited, areaID) == true then 
					for loop, plotIndex in ipairs(area_plot_list) do
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local plot = Map.GetPlot(x, y);
						local terrainType = plot:GetTerrainType();
						if terrainType ~= TerrainTypes.TERRAIN_SNOW then
							if self.plotDataIsCoastal[plotIndex] == true then
								table.insert(self.uninhabited_areas_coastal_plots, plotIndex);
							else
								table.insert(self.uninhabited_areas_inland_plots, plotIndex);
							end
						end
					end
				end
			end
		end

		local uninhabited_ratio = iNumUninhabitedLandmassPlots / (iNumCivLandmassPlots + iNumUninhabitedLandmassPlots);
		local max_by_ratio = math.floor(3 * uninhabited_ratio * self.iNumCityStates);
		local max_by_method;
		if self.method == 1 then
			max_by_method = math.ceil(self.iNumCityStates / 4);
		else
			max_by_method = math.ceil(self.iNumCityStates / 2);
		end
		self.iNumCityStatesUninhabited = math.min(self.iNumCityStatesUnassigned, max_by_ratio, max_by_method);
		self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - self.iNumCityStatesUninhabited;
	end


	current_cs_index = current_cs_index + self.iNumCityStatesUninhabited;
	
	if self.iNumCityStatesUnassigned > 0 then

		local iNumRegionsSharedLux = 0;
		local shared_lux_IDs = {};
		for resource_ID, amount_assigned_to_regions in ipairs(self.luxury_assignment_count) do
			if amount_assigned_to_regions == 3 then
				iNumRegionsSharedLux = iNumRegionsSharedLux + 3;
				table.insert(shared_lux_IDs, resource_ID);
			end
		end
		if iNumRegionsSharedLux > 0 and iNumRegionsSharedLux <= self.iNumCityStatesUnassigned then
			self.iNumCityStatesSharedLux = iNumRegionsSharedLux;
			self.iNumCityStatesLowFertility = self.iNumCityStatesUnassigned - self.iNumCityStatesSharedLux;
		else
			self.iNumCityStatesLowFertility = self.iNumCityStatesUnassigned;
		end


		if self.iNumCityStatesSharedLux > 0 then
			for loop, res_ID in ipairs(shared_lux_IDs) do
				for loop, region_lux_data in ipairs(self.regions_sorted_by_type) do
					local this_region_res = region_lux_data[2];
					if this_region_res == res_ID then
						self.city_state_region_assignments[current_cs_index] = region_lux_data[1];

						current_cs_index = current_cs_index + 1;
						self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
					end
				end
			end
		end
		if self.iNumCityStatesLowFertility > 0 then

			while self.iNumCityStatesUnassigned >= self.iNumCivs do
				for current_region = 1, self.iNumCivs do
					self.city_state_region_assignments[current_cs_index] = current_region;

					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
			if self.iNumCityStatesUnassigned > 0 then
				local fert_unsorted, fert_sorted, region_list = {}, {}, {};
				for region_num = 1, self.iNumCivs do
					local area_plots = self.regionTerrainCounts[region_num][2];
					local region_fertility = self.regionData[region_num][6];
					local fertility_per_land_plot = region_fertility / area_plots;

					
					table.insert(fert_unsorted, {region_num, fertility_per_land_plot});
					table.insert(fert_sorted, fertility_per_land_plot);
				end
				table.sort(fert_sorted);
				for current_lowest_fertility, fert_value in ipairs(fert_sorted) do
					for loop, data_pair in ipairs(fert_unsorted) do
						local this_region_fert = data_pair[2];
						if this_region_fert == fert_value then
							local regionNum = data_pair[1];
							table.insert(region_list, regionNum);
							table.remove(fert_unsorted, loop);
							break
						end
					end
				end
				for loop = 1, self.iNumCityStatesUnassigned do
					self.city_state_region_assignments[current_cs_index] = region_list[loop];

					current_cs_index = current_cs_index + 1;
					self.iNumCityStatesUnassigned = self.iNumCityStatesUnassigned - 1;
				end
			end
		end
	end
	

	if self.iNumCityStatesUnassigned ~= 0 then
		print("Wrong number of City States assigned at end of assignment process. This number unassigned: ", self.iNumCityStatesUnassigned);
	else
		print("All city states assigned.");
	end
end

function AssignStartingPlots:CanPlaceCityStateAt(x, y, area_ID, force_it, ignore_collisions)
	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y)
	local area = plot:GetArea()
	local biggest_area = Map.FindBiggestArea(False);
	local iAreaID = biggest_area:GetID();

	if self.method == 1 then
		if area_ID ~= iAreaID then
			return false
		end
	end

	if area ~= area_ID and area_ID ~= -1 then
		return false
	end
	local plotType = plot:GetPlotType()
	if plotType == PlotTypes.PLOT_OCEAN or plotType == PlotTypes.PLOT_MOUNTAIN then
		return false
	end
	local terrainType = plot:GetTerrainType()
	if terrainType == TerrainTypes.TERRAIN_SNOW then
		return false
	end
	local featureType = plot:GetFeatureType()
	if featureType == FeatureTypes.FEATURE_OASIS then
		return false
	end
	local plotIndex = y * iW + x + 1;
	if self.cityStateData[plotIndex] > 0 and force_it == false then
		return false
	end
	if self.playerCollisionData[plotIndex] == true and ignore_collisions == false then

		return false
	end
	if self.plotDataIsNextToCoast[plotIndex] == true then
		return false
	end
	return true
end

function AssignStartingPlots:ObtainNextSectionInRegion(incoming_west_x, incoming_south_y,
	                         incoming_width, incoming_height, iAreaID, force_it, ignore_collisions)











	local iW, iH = Map.GetGridSize();
	local reached_middle = false;
	if incoming_width <= 0 or incoming_height <= 0 then
		return {}, {}, -1, -1, -1, -1, true;
	end
	if incoming_width < 4 or incoming_height < 4 then
		reached_middle = true;
	end
	local bTaller = false;
	local rows_to_check = math.ceil(0.2 * incoming_width);
	if incoming_height > incoming_width then
		bTaller = true;
		rows_to_check = math.ceil(0.2 * incoming_height);
	end

	local coastal_plots, inland_plots = {}, {};
	for section_y = incoming_south_y, incoming_south_y + incoming_height - 1 do
		for section_x = incoming_west_x, incoming_west_x + incoming_width - 1 do
			if reached_middle then
				local x = section_x % iW;
				local y = section_y % iH;
				if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
					local i = y * iW + x + 1;
					if self.plotDataIsCoastal[i] == true then
						table.insert(coastal_plots, i);
					else
						table.insert(inland_plots, i);
					end
				end
			else
				if bTaller == false then
					if section_x < incoming_west_x + rows_to_check or section_x >= incoming_west_x + incoming_width - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] == true then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				else
					if section_y < incoming_south_y + rows_to_check or section_y >= incoming_south_y + incoming_height - rows_to_check then
						local x = section_x % iW;
						local y = section_y % iH;
						if self:CanPlaceCityStateAt(x, y, iAreaID, force_it, ignore_collisions) == true then
							local i = y * iW + x + 1;
							if self.plotDataIsCoastal[i] == true then
								table.insert(coastal_plots, i);
							else
								table.insert(inland_plots, i);
							end
						end
					end
				end
			end
		end
	end
	local new_west_x, new_south_y, new_width, new_height;
	if bTaller then
		new_west_x = incoming_west_x + rows_to_check;
		new_south_y = incoming_south_y;
		new_width = incoming_width - (2 * rows_to_check);
		new_height = incoming_height;
	else
		new_west_x = incoming_west_x;
		new_south_y = incoming_south_y + rows_to_check;
		new_width = incoming_width;
		new_height = incoming_height - (2 * rows_to_check);
	end		

	return coastal_plots, inland_plots, new_west_x, new_south_y, new_width, new_height, reached_middle;
end

function AssignStartingPlots:PlaceCityState(coastal_plot_list, inland_plot_list, check_proximity, check_collision)


	if coastal_plot_list == nil or inland_plot_list == nil then
		print("Nil plot list incoming for PlaceCityState()");
	end
	local iW, iH = Map.GetGridSize()

	print("------------------------------------- CS PLOTS READOUT -------------------------------------");
	print("Inc. Coastal List Size: ", table.maxn(coastal_plot_list));
	print("Inc. Inland List Size: ", table.maxn(inland_plot_list));
	print("--------------------------------------------------------------------------------------------");

	local coastornot = Map.Rand(100, "Chance for coast v inalnd");

	if coastornot >= 42 then

		local iNumCoastal = table.maxn(coastal_plot_list);
		if iNumCoastal > 0 then
			if check_collision == false then
				local diceroll = 1 + Map.Rand(iNumCoastal, "Standard City State placement - LUA");
				local selected_plot_index = coastal_plot_list[diceroll];
				local x = (selected_plot_index - 1) % iW;
				local y = (selected_plot_index - x - 1) / iW;
				return x, y, true;
			else
				local randomized_coastal = GetShuffledCopyOfTable(coastal_plot_list);
				for loop, candidate_plot in ipairs(randomized_coastal) do

					if self.playerCollisionData[candidate_plot] == false then



						if check_proximity == false or self.cityStateData[candidate_plot] == 0 then
							local x = (candidate_plot - 1) % iW;
							local y = (candidate_plot - x - 1) / iW;
							return x, y, true;
						end
					end
				end
			end
		end

		local iNumInland = table.maxn(inland_plot_list);
		if iNumInland > 0 then
			if check_collision == false then
				local diceroll = 1 + Map.Rand(iNumInland, "Standard City State placement - LUA");
				local selected_plot_index = inland_plot_list[diceroll];
				local x = (selected_plot_index - 1) % iW;
				local y = (selected_plot_index - x - 1) / iW;
				return x, y, true;
			else
				local randomized_inland = GetShuffledCopyOfTable(inland_plot_list);
				for loop, candidate_plot in ipairs(randomized_inland) do
					if self.playerCollisionData[candidate_plot] == false then
						if check_proximity == false or self.cityStateData[candidate_plot] == 0 then
							local x = (candidate_plot - 1) % iW;
							local y = (candidate_plot - x - 1) / iW;
							return x, y, true;
						end
					end
				end
			end
		end

	else

		local iNumInland = table.maxn(inland_plot_list);
		if iNumInland > 0 then
			if check_collision == false then
				local diceroll = 1 + Map.Rand(iNumInland, "Standard City State placement - LUA");
				local selected_plot_index = inland_plot_list[diceroll];
				local x = (selected_plot_index - 1) % iW;
				local y = (selected_plot_index - x - 1) / iW;
				return x, y, true;
			else
				local randomized_inland = GetShuffledCopyOfTable(inland_plot_list);
				for loop, candidate_plot in ipairs(randomized_inland) do
					if self.playerCollisionData[candidate_plot] == false then
						if check_proximity == false or self.cityStateData[candidate_plot] == 0 then
							local x = (candidate_plot - 1) % iW;
							local y = (candidate_plot - x - 1) / iW;
							return x, y, true;
						end
					end
				end
			end
		end

		local iNumCoastal = table.maxn(coastal_plot_list);
		if iNumCoastal > 0 then
			if check_collision == false then
				local diceroll = 1 + Map.Rand(iNumCoastal, "Standard City State placement - LUA");
				local selected_plot_index = coastal_plot_list[diceroll];
				local x = (selected_plot_index - 1) % iW;
				local y = (selected_plot_index - x - 1) / iW;
				return x, y, true;
			else
				local randomized_coastal = GetShuffledCopyOfTable(coastal_plot_list);
				for loop, candidate_plot in ipairs(randomized_coastal) do
					if self.playerCollisionData[candidate_plot] == false then
						if check_proximity == false or self.cityStateData[candidate_plot] == 0 then
							local x = (candidate_plot - 1) % iW;
							local y = (candidate_plot - x - 1) / iW;
							return x, y, true;
						end
					end
				end
			end
		end
	end

	return 0, 0, false;
end

function AssignStartingPlots:PlaceCityStateInRegion(city_state_number, region_number)

	local iW, iH = Map.GetGridSize();
	local placed_city_state = false;
	local reached_middle = false;
	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];

	local eligible_coastal, eligible_inland = {}, {};


	local x, y;
	local curWX = iWestX;
	local curSY = iSouthY;
	local curWid = iWidth;
	local curHei = iHeight;
	while placed_city_state == false and reached_middle == false do

		local nextWX, nextSY, nextWid, nextHei;
		eligible_coastal, eligible_inland, nextWX, nextSY, nextWid, nextHei,
		  reached_middle = self:ObtainNextSectionInRegion(curWX, curSY, curWid, curHei, iAreaID, false, false)
		curWX, curSY, curWid, curHei = nextWX, nextSY, nextWid, nextHei;

		x, y, placed_city_state = self:PlaceCityState(eligible_coastal, eligible_inland, false, false)
	end







































































































	if placed_city_state == true then

		self.cityStatePlots[city_state_number] = {x, y, region_number};
		self.city_state_validity_table[city_state_number] = true;
		local city_state_ID = city_state_number + GameDefines.MAX_MAJOR_CIVS - 1;
		local cityState = Players[city_state_ID];
		local cs_start_plot = Map.GetPlot(x, y)
		cityState:SetStartingPlot(cs_start_plot)
		self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, true)
		self:PlaceResourceImpact(x, y, 5, 4)
		self:PlaceResourceImpact(x, y, 2, 3)
		self:PlaceResourceImpact(x, y, 1, 0)
		self:PlaceResourceImpact(x, y, 3, 3)
		self:PlaceResourceImpact(x, y, 4, 3)
		self:PlaceResourceImpact(x, y, 7, 3)

		local impactPlotIndex = y * iW + x + 1;
		self.playerCollisionData[impactPlotIndex] = true;

	else

		self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
	end
end

function AssignStartingPlots:PlaceCityStates()
	print("Map Generation - Choosing sites for City States");




	self:AssignCityStatesToRegionsOrToUninhabited()
	


	local iW, iH = Map.GetGridSize();
	local iUninhabitedCandidatePlots = table.maxn(self.uninhabited_areas_coastal_plots) + table.maxn(self.uninhabited_areas_inland_plots);

	for cs_number, region_number in ipairs(self.city_state_region_assignments) do
		if cs_number <= self.iNumCityStates then
			if region_number == -1 and iUninhabitedCandidatePlots > 0 then

				iUninhabitedCandidatePlots = iUninhabitedCandidatePlots - 1;
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(self.uninhabited_areas_coastal_plots, self.uninhabited_areas_inland_plots, true, true)












				if success == true then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true;
					local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y)
					cityState:SetStartingPlot(cs_start_plot)
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true)
					self:PlaceResourceImpact(cs_x, cs_y, 5, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 2, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 1, 0)
					self:PlaceResourceImpact(cs_x, cs_y, 3, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 4, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 7, 3)

					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;

				else

					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded + 1;
				end
			elseif region_number == -1 and iUninhabitedCandidatePlots <= 0 then
				local iRandRegion = 1 + Map.Rand(self.iNumCivs, "Emergency Redirect of CS placement, choosing Region - LUA");

				self:PlaceCityStateInRegion(cs_number, iRandRegion)
			else

				self:PlaceCityStateInRegion(cs_number, region_number)
			end
		end
	end
	

	if self.iNumCityStatesDiscarded > 0 then

		local cs_last_chance_plot_list = {};
		for y = 0, iH - 1 do
			for x = 0, iW - 1 do
				if self:CanPlaceCityStateAt(x, y, -1, false, false) == true then
					local i = y * iW + x + 1;
					table.insert(cs_last_chance_plot_list, i);
				end
			end
		end
		local iNumLastChanceCandidates = table.maxn(cs_last_chance_plot_list);

		if iNumLastChanceCandidates > 0 then
			print("-"); print("-"); print("ALERT: Some City States failed to be placed due to overcrowding. Attempting 'last chance' placement method.");
			print("Total number of remaining eligible candidate plots:", iNumLastChanceCandidates);
			local last_chance_shuffled = GetShuffledCopyOfTable(cs_last_chance_plot_list)
			local cs_list = {};
			for cs_num = 1, self.iNumCityStates do
				if self.city_state_validity_table[cs_num] == false then
					table.insert(cs_list, cs_num);

				end
			end
			for loop, cs_number in ipairs(cs_list) do
				local cs_x, cs_y, success;
				cs_x, cs_y, success = self:PlaceCityState(last_chance_shuffled, {}, true, true)
				if success == true then
					self.cityStatePlots[cs_number] = {cs_x, cs_y, -1};
					self.city_state_validity_table[cs_number] = true;
					local city_state_ID = cs_number + GameDefines.MAX_MAJOR_CIVS - 1;
					local cityState = Players[city_state_ID];
					local cs_start_plot = Map.GetPlot(cs_x, cs_y)
					cityState:SetStartingPlot(cs_start_plot)
					self:GenerateLuxuryPlotListsAtCitySite(cs_x, cs_y, 1, true)
					self:PlaceResourceImpact(cs_x, cs_y, 5, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 2, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 1, 0)
					self:PlaceResourceImpact(cs_x, cs_y, 3, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 4, 3)
					self:PlaceResourceImpact(cs_x, cs_y, 7, 3)

					local impactPlotIndex = cs_y * iW + cs_x + 1;
					self.playerCollisionData[impactPlotIndex] = true;
					self.iNumCityStatesDiscarded = self.iNumCityStatesDiscarded - 1;

				else

					break
				end
			end
			if self.iNumCityStatesDiscarded > 0 then
				print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
			end
		else
			print("-"); print("-"); print("ALERT: No eligible city state sites remain. DISCARDING", self.iNumCityStatesDiscarded, "city states. BYE BYE!"); print("-");
		end
	end
end

function AssignStartingPlots:NormalizeCityState(x, y)

	local iW, iH = Map.GetGridSize();
	local plot = Map.GetPlot(x, y);
	local isEvenY = true;
	if y / 2 > math.floor(y / 2) then
		isEvenY = false;
	end
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local innerFourFood, innerThreeFood, innerTwoFood, innerHills, innerForest, innerOneHammer, innerOcean = 0, 0, 0, 0, 0, 0, 0;
	local outerFourFood, outerThreeFood, outerTwoFood, outerOcean = 0, 0, 0, 0;
	local innerCanHaveBonus, outerCanHaveBonus, innerBadTiles, outerBadTiles = 0, 0, 0, 0;
	local iNumFoodBonusNeeded = 0;
	local search_table = {};
	









	if isEvenY then
		search_table = self.firstRingYIsEven;
	else
		search_table = self.firstRingYIsOdd;
	end

	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			innerBadTiles = innerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				innerBadTiles = innerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerTwoFood = innerTwoFood + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						innerBadTiles = innerBadTiles + 1;
					else
						innerOcean = innerOcean + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				end
			else
				if plotType == PlotTypes.PLOT_HILLS then
					innerHills = innerHills + 1;
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						innerCanHaveBonus = innerCanHaveBonus + 1;
					end
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerFourFood = innerFourFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						innerFourFood = innerFourFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerThreeFood = innerThreeFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					else
						innerBadTiles = innerBadTiles + 1;
					end
				else
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						innerThreeFood = innerThreeFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							innerCanHaveBonus = innerCanHaveBonus + 1;
						end
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						innerTwoFood = innerTwoFood + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerOneHammer = innerOneHammer + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						innerCanHaveBonus = innerCanHaveBonus + 1;
						if featureType == FeatureTypes.FEATURE_FOREST then
							innerForest = innerForest + 1;
						else
							innerBadTiles = innerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						innerBadTiles = innerBadTiles + 1;
						innerCanHaveBonus = innerCanHaveBonus + 1;
					else
						innerBadTiles = innerBadTiles + 1;
					end
				end
			end
		end
	end
				

	if isEvenY then
		search_table = self.secondRingYIsEven;
	else
		search_table = self.secondRingYIsOdd;
	end
	for loop, plot_adjustments in ipairs(search_table) do
		local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)
		if searchX < 0 or searchX >= iW or searchY < 0 or searchY >= iH then
			outerBadTiles = outerBadTiles + 1;
		else
			local searchPlot = Map.GetPlot(searchX, searchY)
			local plotType = searchPlot:GetPlotType()
			local terrainType = searchPlot:GetTerrainType()
			local featureType = searchPlot:GetFeatureType()

			if plotType == PlotTypes.PLOT_MOUNTAIN then
				outerBadTiles = outerBadTiles + 1;
			elseif plotType == PlotTypes.PLOT_OCEAN then
				if searchPlot:IsLake() then
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					else
						outerTwoFood = outerTwoFood + 1;
					end
				else
					if featureType == FeatureTypes.FEATURE_ICE then
						outerBadTiles = outerBadTiles + 1;
					elseif terrainType == TerrainTypes.TERRAIN_COAST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						outerOcean = outerOcean + 1;
					end
				end
			else
				if plotType == PlotTypes.PLOT_HILLS then
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						outerCanHaveBonus = outerCanHaveBonus + 1;
					end
				elseif searchPlot:IsFreshWater() then
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerFourFood = outerFourFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
					elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
						outerFourFood = outerFourFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerThreeFood = outerThreeFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					else
						outerBadTiles = outerBadTiles + 1;
					end
				else
					if terrainType == TerrainTypes.TERRAIN_GRASS then
						outerThreeFood = outerThreeFood + 1;
						if featureType ~= FeatureTypes.FEATURE_MARSH then
							outerCanHaveBonus = outerCanHaveBonus + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						outerTwoFood = outerTwoFood + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
						outerCanHaveBonus = outerCanHaveBonus + 1;
						if featureType ~= FeatureTypes.FEATURE_FOREST then
							outerBadTiles = outerBadTiles + 1;
						end
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						outerBadTiles = outerBadTiles + 1;
						outerCanHaveBonus = outerCanHaveBonus + 1;
					else
						outerBadTiles = outerBadTiles + 1;
					end
				end
			end
		end
	end
	

	local hammerScore = (4 * innerHills) + (2 * innerForest) + innerOneHammer;
	if hammerScore < 6 then
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
		end
		for attempt = 1, 6 do
			local plot_adjustments = randomized_first_ring_adjustments[attempt];
			local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

			local placedHill = self:AttemptToPlaceHillsAtPlot(searchX, searchY);
			if placedHill == true then
				hammerScore = hammerScore + 4;

				break
			elseif attempt == 6 then

			end
		end
	end
	

	local innerFoodScore = (4 * innerFourFood) + (2 * innerThreeFood) + innerTwoFood;
	local outerFoodScore = (4 * outerFourFood) + (2 * outerThreeFood) + outerTwoFood;
	local totalFoodScore = innerFoodScore + outerFoodScore;





	

	iNumFoodBonusNeeded = 1;
	if totalFoodScore < 8 or innerFoodScore < 4 then
		iNumFoodBonusNeeded = 3;
	elseif totalFoodScore < 12 and innerFoodScore < 9 then
		iNumFoodBonusNeeded = 2;
	end

	if iNumFoodBonusNeeded > 0 then
		local maxBonusesPossible = innerCanHaveBonus + outerCanHaveBonus;





		local innerPlaced, outerPlaced = 0, 0;
		local randomized_first_ring_adjustments, randomized_second_ring_adjustments;
		if isEvenY then
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsEven);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsEven);
		else
			randomized_first_ring_adjustments = GetShuffledCopyOfTable(self.firstRingYIsOdd);
			randomized_second_ring_adjustments = GetShuffledCopyOfTable(self.secondRingYIsOdd);
		end
		local tried_all_first_ring = false;
		local tried_all_second_ring = false;
		local allow_oasis = true;
		local allow_fishCount = 2;
		local placedOasis;
		while iNumFoodBonusNeeded > 0 do
			if innerPlaced < 2 and innerCanHaveBonus > 0 and tried_all_first_ring == false then
				for attempt = 1, 6 do
					local plot_adjustments = randomized_first_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus, placedOasis, placedFish = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis, allow_fishCount);
					if placedBonus == true then
						if placedFish == true then
							allow_fishCount = allow_fishCount - 1;
						end
						if allow_oasis == true and placedOasis == true then
							allow_oasis = false;
						end

						innerPlaced = innerPlaced + 1;
						innerCanHaveBonus = innerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 6 then
						tried_all_first_ring = true;
					end
				end

			elseif innerPlaced + outerPlaced < 4 and outerCanHaveBonus > 0 and tried_all_second_ring == false then

				for attempt = 1, 12 do
					local plot_adjustments = randomized_second_ring_adjustments[attempt];
					local searchX, searchY = self:ApplyHexAdjustment(x, y, plot_adjustments)

					local placedBonus, placedOasis, placedFish = self:AttemptToPlaceBonusResourceAtPlot(searchX, searchY, allow_oasis, allow_fishCount);
					if placedBonus == true then
						if placedFish == true then
							allow_fishCount = allow_fishCount - 1;
						end
						if allow_oasis == true and placedOasis == true then
							allow_oasis = false;
						end

						outerPlaced = outerPlaced + 1;
						outerCanHaveBonus = outerCanHaveBonus - 1;
						iNumFoodBonusNeeded = iNumFoodBonusNeeded - 1;
						break
					elseif attempt == 12 then
						tried_all_second_ring = true;
					end
				end
			
			else
				break				
			end
		end
	end
end

function AssignStartingPlots:NormalizeCityStateLocations()
	for city_state, data_table in ipairs(self.cityStatePlots) do
		if self.city_state_validity_table[city_state] == true then
			local x = data_table[1];
			local y = data_table[2];
			self:NormalizeCityState(x, y)
		else
			print("WARNING: City State #", city_state, "is not valid in this game. It must have been discarded from overcrowding.");
		end
	end
end



function AssignStartingPlots:GenerateGlobalResourcePlotLists()

	local iW, iH = Map.GetGridSize();
	local temp_coast_next_to_land_list, temp_marsh_list, temp_flood_plains_list = {}, {}, {};
	local temp_hills_open_list, temp_hills_covered_list, temp_hills_jungle_list = {}, {}, {};
	local temp_hills_forest_list, temp_jungle_flat_list, temp_forest_flat_list = {}, {}, {};
	local temp_desert_flat_no_feature, temp_plains_flat_no_feature, temp_dry_grass_flat_no_feature = {}, {}, {};
	local temp_fresh_water_grass_flat_no_feature, temp_tundra_flat_including_forests, temp_forest_flat_that_are_not_tundra = {}, {}, {};
	local temp_dry_plains_flat_no_feature, temp_fresh_water_plains_flat_no_feature = {}, {};
	local temp_desert_or_tundra_flat_no_feature, temp_tundra_flat_forest = {}, {};
	local temp_hills_open_no_tundra, temp_hills_open_no_desert, temp_hills_open_no_tundra_no_desert = {}, {}, {};
	local temp_hills_open_no_grass, temp_hills_open_no_grass_no_tundra, temp_hills_covered_no_tundra = {}, {}, {};
	local temp_hills_covered_no_grass, temp_hills_covered_no_grass_no_tundra, temp_flat_covered = {}, {}, {};
	local temp_flat_covered_no_grass, temp_flat_covered_no_tundra, temp_flat_covered_no_grass_no_tundra = {}, {}, {};
	local temp_flat_open, temp_flat_open_no_grass_no_plains, temp_flat_open_no_tundra_no_desert = {}, {}, {};
	local temp_flat_open_no_desert, temp_flat_desert_including_flood, temp_hills_open_no_grass_no_plains = {}, {}, {};

	local temp_hills_list, temp_coast_list, temp_grass_flat_no_feature = {}, {}, {};
	local temp_tundra_flat_no_feature, temp_snow_flat_list, temp_land_list = {}, {}, {}, {};
	local temp_marble_list, temp_deer_list, temp_desert_wheat_list, temp_banana_list = {}, {}, {}, {};

	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			local i = y * iW + x + 1;
			local plot = Map.GetPlot(x, y)

			if self.playerCollisionData[i] == true then

			elseif plot:GetResourceType(-1) ~= -1 then

			else

				local plotType = plot:GetPlotType()
				local terrainType = plot:GetTerrainType()
				local featureType = plot:GetFeatureType()
				if plotType == PlotTypes.PLOT_MOUNTAIN then
					self.barren_plots = self.barren_plots + 1;
				elseif plotType == PlotTypes.PLOT_OCEAN then
					if featureType ~= self.feature_atoll then
						if featureType == FeatureTypes.FEATURE_ICE then
							self.barren_plots = self.barren_plots + 1;
						elseif plot:IsLake() then
							self.barren_plots = self.barren_plots + 1;
						elseif terrainType == TerrainTypes.TERRAIN_COAST then
							table.insert(temp_coast_list, i);
							if plot:IsAdjacentToLand() then
								table.insert(temp_coast_next_to_land_list, i);
							end
						else
							self.barren_plots = self.barren_plots + 1;
						end
					end
				elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
					table.insert(temp_hills_list, i);
					if featureType == FeatureTypes.NO_FEATURE then
						table.insert(temp_hills_open_list, i);
						table.insert(temp_marble_list, i);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(temp_hills_open_no_desert, i);
							table.insert(temp_hills_open_no_grass, i);
							table.insert(temp_hills_open_no_grass_no_plains, i);
						elseif terrainType == TerrainTypes.TERRAIN_DESERT then
							table.insert(temp_hills_open_no_tundra, i);
							table.insert(temp_hills_open_no_grass, i);
							table.insert(temp_hills_open_no_grass_no_tundra, i);
							table.insert(temp_hills_open_no_grass_no_plains, i);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_hills_open_no_tundra, i);
							table.insert(temp_hills_open_no_desert, i);
							table.insert(temp_hills_open_no_grass, i);
							table.insert(temp_hills_open_no_grass_no_tundra, i);
							table.insert(temp_hills_open_no_tundra_no_desert, i);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_hills_open_no_tundra, i);
							table.insert(temp_hills_open_no_desert, i);
							table.insert(temp_hills_open_no_tundra_no_desert, i);
						else
							self.barren_plots = self.barren_plots + 1;
							table.remove(temp_hills_list);
						end
					elseif featureType == FeatureTypes.FEATURE_JUNGLE then
						table.insert(temp_banana_list, i);
						table.insert(temp_hills_jungle_list, i);
						table.insert(temp_hills_covered_list, i);
						if terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_hills_covered_no_tundra, i);
							table.insert(temp_hills_covered_no_grass, i);
							table.insert(temp_hills_covered_no_grass_no_tundra, i);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_hills_covered_no_tundra, i);
						end
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						table.insert(temp_hills_forest_list, i);
						table.insert(temp_hills_covered_list, i);
						table.insert(temp_marble_list, i);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(temp_deer_list, i);
							table.insert(temp_hills_covered_no_grass, i);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_hills_covered_no_tundra, i);
							table.insert(temp_hills_covered_no_grass, i);
							table.insert(temp_hills_covered_no_grass_no_tundra, i);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_hills_covered_no_tundra, i);
						end
					else
						self.barren_plots = self.barren_plots + 1;
						table.remove(temp_hills_list);
					end
				elseif featureType == FeatureTypes.FEATURE_MARSH then
					table.insert(temp_marsh_list, i);
				elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
					table.insert(temp_flood_plains_list, i);
					table.insert(temp_desert_wheat_list, i);
					table.insert(temp_flat_desert_including_flood, i);
				elseif plotType == PlotTypes.PLOT_LAND then
					table.insert(temp_land_list, i);
					if featureType == FeatureTypes.FEATURE_JUNGLE then
						table.insert(temp_jungle_flat_list, i);
						table.insert(temp_banana_list, i);
						table.insert(temp_flat_covered, i);
						if terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_flat_covered_no_tundra, i);
							table.insert(temp_flat_covered_no_grass, i);
							table.insert(temp_flat_covered_no_grass_no_tundra, i);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_flat_covered_no_tundra, i);
						end
					elseif featureType == FeatureTypes.FEATURE_FOREST then
						table.insert(temp_forest_flat_list, i);
						table.insert(temp_flat_covered, i);
						if terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(temp_deer_list, i);
							table.insert(temp_tundra_flat_including_forests, i);

							table.insert(temp_tundra_flat_forest, i);
							table.insert(temp_flat_covered_no_grass, i);
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_forest_flat_that_are_not_tundra, i);
							table.insert(temp_flat_covered_no_tundra, i);
							table.insert(temp_flat_covered_no_grass_no_tundra, i);
							table.insert(temp_flat_covered_no_grass, i);
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_forest_flat_that_are_not_tundra, i);
							table.insert(temp_flat_covered_no_tundra, i);
						end
					elseif featureType == FeatureTypes.NO_FEATURE then
						if terrainType == TerrainTypes.TERRAIN_SNOW then
							table.insert(temp_snow_flat_list, i);
						elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
							table.insert(temp_tundra_flat_no_feature, i);
							table.insert(temp_tundra_flat_including_forests, i);
							table.insert(temp_marble_list, i);
							table.insert(temp_desert_or_tundra_flat_no_feature, i);
							table.insert(temp_flat_open, i);
							table.insert(temp_flat_open_no_desert, i);
							table.insert(temp_flat_open_no_grass_no_plains, i);
						elseif terrainType == TerrainTypes.TERRAIN_DESERT then
							table.insert(temp_desert_flat_no_feature, i);
							table.insert(temp_marble_list, i);
							table.insert(temp_desert_or_tundra_flat_no_feature, i);
							table.insert(temp_flat_open, i);
							table.insert(temp_flat_open_no_grass_no_plains, i);
							table.insert(temp_flat_desert_including_flood, i);
							if plot:IsFreshWater() then
								table.insert(temp_desert_wheat_list, i);
							end
						elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
							table.insert(temp_plains_flat_no_feature, i);
							table.insert(temp_marble_list, i);
							table.insert(temp_flat_open_no_desert, i);
							table.insert(temp_flat_open, i);
							table.insert(temp_flat_open_no_tundra_no_desert, i);
							if plot:IsFreshWater() then
								table.insert(temp_fresh_water_plains_flat_no_feature, i);
							else
								table.insert(temp_dry_plains_flat_no_feature, i);
							end
						elseif terrainType == TerrainTypes.TERRAIN_GRASS then
							table.insert(temp_grass_flat_no_feature, i);
							table.insert(temp_marble_list, i);
							table.insert(temp_flat_open_no_desert, i);
							table.insert(temp_flat_open, i);
							table.insert(temp_flat_open_no_tundra_no_desert, i);
							if plot:IsFreshWater() then
								table.insert(temp_fresh_water_grass_flat_no_feature, i);
							else
								table.insert(temp_dry_grass_flat_no_feature, i);
								table.insert(temp_marble_list, i);
							end
						else
							self.barren_plots = self.barren_plots + 1;
							table.remove(temp_land_list);
						end
					else
						self.barren_plots = self.barren_plots + 1;
						table.remove(temp_land_list);
					end
				else
					self.barren_plots = self.barren_plots + 1;
				end
			end
		end
	end

	self.coast_next_to_land_list = GetShuffledCopyOfTable(temp_coast_next_to_land_list)
	self.marsh_list = GetShuffledCopyOfTable(temp_marsh_list)
	self.flood_plains_list = GetShuffledCopyOfTable(temp_flood_plains_list)
	self.hills_open_list = GetShuffledCopyOfTable(temp_hills_open_list)
	self.hills_covered_list = GetShuffledCopyOfTable(temp_hills_covered_list)
	self.hills_jungle_list = GetShuffledCopyOfTable(temp_hills_jungle_list)
	self.hills_forest_list = GetShuffledCopyOfTable(temp_hills_forest_list)
	self.jungle_flat_list = GetShuffledCopyOfTable(temp_jungle_flat_list)
	self.forest_flat_list = GetShuffledCopyOfTable(temp_forest_flat_list)
	self.desert_flat_no_feature = GetShuffledCopyOfTable(temp_desert_flat_no_feature)
	self.plains_flat_no_feature = GetShuffledCopyOfTable(temp_plains_flat_no_feature)
	self.dry_grass_flat_no_feature = GetShuffledCopyOfTable(temp_dry_grass_flat_no_feature)
	self.fresh_water_grass_flat_no_feature = GetShuffledCopyOfTable(temp_fresh_water_grass_flat_no_feature)
	self.tundra_flat_including_forests = GetShuffledCopyOfTable(temp_tundra_flat_including_forests)
	self.forest_flat_that_are_not_tundra = GetShuffledCopyOfTable(temp_forest_flat_that_are_not_tundra)
	self.dry_plains_flat_no_feature = GetShuffledCopyOfTable(temp_dry_plains_flat_no_feature)
	self.fresh_water_plains_flat_no_feature = GetShuffledCopyOfTable(temp_fresh_water_plains_flat_no_feature)
	self.desert_or_tundra_flat_no_feature = GetShuffledCopyOfTable(temp_desert_or_tundra_flat_no_feature)
	self.tundra_flat_forest = GetShuffledCopyOfTable(temp_tundra_flat_forest)
	self.hills_open_no_tundra = GetShuffledCopyOfTable(temp_hills_open_no_tundra)
	self.hills_open_no_desert = GetShuffledCopyOfTable(temp_hills_open_no_desert)
	self.hills_open_no_tundra_no_desert = GetShuffledCopyOfTable(temp_hills_open_no_tundra_no_desert)
	self.hills_open_no_grass = GetShuffledCopyOfTable(temp_hills_open_no_grass)
	self.hills_open_no_grass_no_tundra = GetShuffledCopyOfTable(temp_hills_open_no_grass_no_tundra)
	self.hills_open_no_grass_no_plains = GetShuffledCopyOfTable(temp_hills_open_no_grass_no_plains)
	self.hills_covered_no_tundra = GetShuffledCopyOfTable(temp_hills_covered_no_tundra)
	self.hills_covered_no_grass = GetShuffledCopyOfTable(temp_hills_covered_no_grass)
	self.hills_covered_no_grass_no_tundra = GetShuffledCopyOfTable(temp_hills_covered_no_grass_no_tundra)
	self.flat_covered = GetShuffledCopyOfTable(temp_flat_covered)
	self.flat_covered_no_grass = GetShuffledCopyOfTable(temp_flat_covered_no_grass)
	self.flat_covered_no_tundra = GetShuffledCopyOfTable(temp_flat_covered_no_tundra)
	self.flat_covered_no_grass_no_tundra = GetShuffledCopyOfTable(temp_flat_covered_no_grass_no_tundra)
	self.flat_open = GetShuffledCopyOfTable(temp_flat_open)
	self.flat_open_no_grass_no_plains = GetShuffledCopyOfTable(temp_flat_open_no_grass_no_plains)
	self.flat_open_no_tundra_no_desert = GetShuffledCopyOfTable(temp_flat_open_no_tundra_no_desert)
	self.flat_open_no_desert = GetShuffledCopyOfTable(temp_flat_open_no_desert)
	self.flat_desert_including_flood = GetShuffledCopyOfTable(temp_flat_desert_including_flood)

	self.grass_flat_no_feature = GetShuffledCopyOfTable(temp_grass_flat_no_feature)
	self.tundra_flat_no_feature = GetShuffledCopyOfTable(temp_tundra_flat_no_feature)
	self.snow_flat_list = GetShuffledCopyOfTable(temp_snow_flat_list)
	self.hills_list = GetShuffledCopyOfTable(temp_hills_list)
	self.land_list = GetShuffledCopyOfTable(temp_land_list)
	self.coast_list = GetShuffledCopyOfTable(temp_coast_list)
	self.marble_list = GetShuffledCopyOfTable(temp_marble_list)
	self.extra_deer_list = GetShuffledCopyOfTable(temp_deer_list)
	self.desert_wheat_list = GetShuffledCopyOfTable(temp_desert_wheat_list)
	self.banana_list = GetShuffledCopyOfTable(temp_banana_list)


	self.global_luxury_plot_lists = {
	self.coast_next_to_land_list,
	self.marsh_list,
	self.flood_plains_list,
	self.hills_open_list,
	self.hills_covered_list,
	self.hills_jungle_list,
	self.hills_forest_list,
	self.jungle_flat_list,
	self.forest_flat_list,
	self.desert_flat_no_feature,
	self.plains_flat_no_feature,
	self.dry_grass_flat_no_feature,
	self.fresh_water_grass_flat_no_feature,
	self.tundra_flat_including_forests,
	self.forest_flat_that_are_not_tundra,
	self.grass_flat_no_feature,
	self.tundra_flat_no_feature,
	self.dry_plains_flat_no_feature,
	self.fresh_water_plains_flat_no_feature,
	self.desert_or_tundra_flat_no_feature,
	self.tundra_flat_forest,
	self.hills_open_no_tundra,
	self.hills_open_no_desert,
	self.hills_open_no_tundra_no_desert,
	self.hills_open_no_grass,
	self.hills_open_no_grass_no_tundra,
	self.hills_open_no_grass_no_plains,
	self.hills_covered_no_tundra,
	self.hills_covered_no_grass,
	self.hills_covered_no_grass_no_tundra,
	self.flat_covered,
	self.flat_covered_no_grass,
	self.flat_covered_no_tundra,
	self.flat_covered_no_grass_no_tundra,
	self.flat_open,
	self.flat_open_no_grass_no_plains,
	self.flat_open_no_tundra_no_desert,
	self.flat_open_no_desert,
	self.flat_desert_including_flood,
	};

end

function AssignStartingPlots:ExpandCoastalRing(x, y, radius)





	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local impactPlotIndex = y * iW + x + 1;

	self.cityStateData[impactPlotIndex] = impact_value;

	if radius == 0 then
		return
	end

	if radius > 0 and radius < iH / 2 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;





			local currentX = x - ripple_radius;
			local currentY = y;



			for direction_index = 1, 6 do
				for plot_to_handle = 1, ripple_radius do

                    if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end

					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];

					if wrapX == false and (nextX < 0 or nextX >= iW) then

					elseif wrapY == false and (nextY < 0 or nextY >= iH) then

					else

						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end

						local ringPlotIndex = realY * iW + realX + 1;

						self.cityStateData[ringPlotIndex] = 1;
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end
end








function AssignStartingPlots:PlaceResourceImpactCoastalMod(x, y, impact_table_number, radius, radiusCoastal)



	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local impactPlotIndex = y * iW + x + 1;

	self.cityStateData[impactPlotIndex] = impact_value;

	if radius == 0 then
		return
	end

	if radius > 0 and radius < iH / 2 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;





			local currentX = x - ripple_radius;
			local currentY = y;



			for direction_index = 1, 6 do
				for plot_to_handle = 1, ripple_radius do

                    if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end

					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];

					if wrapX == false and (nextX < 0 or nextX >= iW) then

					elseif wrapY == false and (nextY < 0 or nextY >= iH) then

					else

						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end

						local ringPlotIndex = realY * iW + realX + 1;

						self.cityStateData[ringPlotIndex] = 1;
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end

	if radiusCoastal > 0 and radiusCoastal < iH / 2 then
		for ripple_radius = 1, radiusCoastal do
			local ripple_value = radiusCoastal - ripple_radius + 1;





			local currentX = x - ripple_radius;
			local currentY = y;



			for direction_index = 1, 6 do
				for plot_to_handle = 1, ripple_radius do

                    if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end

					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];
					local plot = Map.GetPlot(nextX, nextY);
					if plot:IsCoastalLand() then

						if wrapX == false and (nextX < 0 or nextX >= iW) then

						elseif wrapY == false and (nextY < 0 or nextY >= iH) then

						else

							local realX = nextX;
							local realY = nextY;
							if wrapX then
								realX = realX % iW;
							end
							if wrapY then
								realY = realY % iH;
							end

							local ringPlotIndex = realY * iW + realX + 1;

							self.cityStateData[ringPlotIndex] = 1;


							self:ExpandCoastalRing(nextX, nextY, 3)
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported RadiusCoastal length of ", radiusCoastal, " passed to PlaceResourceImpact()");
	end
end


function AssignStartingPlots:PlaceResourceImpact(x, y, impact_table_number, radius)



	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local impact_value = 99;
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local impactPlotIndex = y * iW + x + 1;
	if impact_table_number == 1 then
		self.strategicData[impactPlotIndex] = impact_value;
	elseif impact_table_number == 2 then
		self.luxuryData[impactPlotIndex] = impact_value;
	elseif impact_table_number == 3 then
		self.bonusData[impactPlotIndex] = impact_value;
	elseif impact_table_number == 4 then
		self.fishData[impactPlotIndex] = 99;
	elseif impact_table_number == 5 then
		self.cityStateData[impactPlotIndex] = impact_value;
	elseif impact_table_number == 6 then
		self.naturalWondersData[impactPlotIndex] = impact_value;
	elseif impact_table_number == 7 then
		self.marbleData[impactPlotIndex] = 1;
	elseif impact_table_number == 8 then
		self.seaOilData[impactPlotIndex] = 99;
	end
	if radius == 0 then
		return
	end

	if radius > 0 and radius < iH / 2 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;





			local currentX = x - ripple_radius;
			local currentY = y;



			for direction_index = 1, 6 do
				for plot_to_handle = 1, ripple_radius do

                    if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end

					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];

					if wrapX == false and (nextX < 0 or nextX >= iW) then

					elseif wrapY == false and (nextY < 0 or nextY >= iH) then

					else

						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end

						local ringPlotIndex = realY * iW + realX + 1;
						if impact_table_number == 1 then
							if self.strategicData[ringPlotIndex] > 0 then

								local stronger_value = math.max(self.strategicData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(50, stronger_value + 2);
								self.strategicData[ringPlotIndex] = overlap_value;
							else
								self.strategicData[ringPlotIndex] = ripple_value;
							end
						elseif impact_table_number == 2 then
							if self.luxuryData[ringPlotIndex] > 0 then

								local stronger_value = math.max(self.luxuryData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(50, stronger_value + 2);
								self.luxuryData[ringPlotIndex] = overlap_value;
							else
								self.luxuryData[ringPlotIndex] = ripple_value;
							end
						elseif impact_table_number == 3 then
							if self.bonusData[ringPlotIndex] > 0 then

								local stronger_value = math.max(self.bonusData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(50, stronger_value + 2);
								self.bonusData[ringPlotIndex] = overlap_value;
							else
								self.bonusData[ringPlotIndex] = ripple_value;
							end
						elseif impact_table_number == 4 then
							if self.fishData[ringPlotIndex] > 0 then

								local stronger_value = math.max(self.fishData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(10, stronger_value + 2);
								self.fishData[ringPlotIndex] = overlap_value;
							else
								self.fishData[ringPlotIndex] = ripple_value;
							end
						elseif impact_table_number == 5 then
							self.cityStateData[ringPlotIndex] = 1;
						elseif impact_table_number == 6 then
							if self.naturalWondersData[ringPlotIndex] > 0 then

								local stronger_value = math.max(self.naturalWondersData[ringPlotIndex], ripple_value);

								local overlap_value = math.min(50, stronger_value + 2);
								self.naturalWondersData[ringPlotIndex] = overlap_value;
							else
								self.naturalWondersData[ringPlotIndex] = ripple_value;
							end
						elseif impact_table_number == 7 then
							self.marbleData[ringPlotIndex] = 1;
						elseif impact_table_number == 8 then
							self.seaOilData[ringPlotIndex] = 1;
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	else
		print("Unsupported Radius length of ", radius, " passed to PlaceResourceImpact()");
	end
end

function AssignStartingPlots:ProcessResourceList(frequency, impact_table_number, plot_list, resources_to_place)
















	if plot_list == nil then

		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumResourcesToPlace = math.ceil(iNumTotalPlots / frequency);
	local iNumResourcesTypes = table.maxn(resources_to_place);
	local res_ID, res_quantity, res_weight, res_min, res_max, res_range, res_threshold = {}, {}, {}, {}, {}, {}, {};
	local totalWeight, accumulatedWeight = 0, 0;
	for index, resource_data in ipairs(resources_to_place) do
		res_ID[index] = resource_data[1];
		res_quantity[index] = resource_data[2];
		res_weight[index] = resource_data[3];
		totalWeight = totalWeight + resource_data[3];
		res_min[index] = resource_data[4];
		res_max[index] = resource_data[5];
		if res_max[index] > res_min[index] then
			res_range[index] = res_max[index] - res_min[index] + 1;
		else
			res_range[index] = -1;
		end
	end
	for index = 1, iNumResourcesTypes do




		local threshold = (res_weight[index] + accumulatedWeight) * 10000 / totalWeight;
		table.insert(res_threshold, threshold);
		accumulatedWeight = accumulatedWeight + res_weight[index];
	end

	local current_index = 1;
	local avoid_ripples = true;
	for place_resource = 1, iNumResourcesToPlace do
		local placed_this_res = false;
		local use_this_res_index = 1;
		local diceroll = Map.Rand(10000, "Choose resource type - Distribute Resources - Lua");
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then
				use_this_res_index = index;
				break
			end
		end
		if avoid_ripples == true then
			for index_to_check = current_index, iNumTotalPlots do
				if index_to_check == iNumTotalPlots then
					avoid_ripples = false;
				end
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if impact_table_number == 1 then
					if self.strategicData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end

							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							if (Game.GetResourceUsageType(res_ID[use_this_res_index]) == ResourceUsageTypes.RESOURCEUSAGE_LUXURY) then
								self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
							end
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + res_quantity[use_this_res_index];
						end
					end
				elseif impact_table_number == 2 then
					if self.luxuryData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end

							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + 1;
						end
					end
				elseif impact_table_number == 3 then
					if self.bonusData[plotIndex] == 0 then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							local res_addition = 0;
							if res_range[use_this_res_index] ~= -1 then
								res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
							end

							res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
							self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
							placed_this_res = true;
							self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + 1;
						end
					end
				end
			end
		end
		if avoid_ripples == false then
			local lowest_impact = 98;
			local best_plot;
			for loop, plotIndex in ipairs(plot_list) do
				if impact_table_number == 1 then
					if lowest_impact > self.strategicData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							lowest_impact = self.strategicData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 2 then
					if lowest_impact > self.luxuryData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							lowest_impact = self.luxuryData[plotIndex];
							best_plot = plotIndex;
						end
					end
				elseif impact_table_number == 3 then
					if lowest_impact > self.bonusData[plotIndex] then
						local x = (plotIndex - 1) % iW;
						local y = (plotIndex - x - 1) / iW;
						local res_plot = Map.GetPlot(x, y)
						if res_plot:GetResourceType(-1) == -1 then
							lowest_impact = self.bonusData[plotIndex];
							best_plot = plotIndex;
						end
					end
				end
			end
			if best_plot ~= nil then
				local x = (best_plot - 1) % iW;
				local y = (best_plot - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				local res_addition = 0;
				if res_range[use_this_res_index] ~= -1 then
					res_addition = Map.Rand(res_range[use_this_res_index], "Resource Radius - Place Resource LUA");
				end

				res_plot:SetResourceType(res_ID[use_this_res_index], res_quantity[use_this_res_index]);
				self:PlaceResourceImpact(x, y, impact_table_number, res_min[use_this_res_index] + res_addition);
				self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] = self.amounts_of_resources_placed[res_ID[use_this_res_index] + 1] + res_quantity[use_this_res_index];
			end
		end
	end
end

function AssignStartingPlots:PlaceSpecificNumberOfResources(resource_ID, quantity, amount,
	                         ratio, impact_table_number, min_radius, max_radius, plot_list)
























	
	if plot_list == nil then

		return
	end
	local bCheckImpact = false;
	local impact_table = {};
	if impact_table_number == 1 then
		bCheckImpact = true;
		impact_table = self.strategicData;
	elseif impact_table_number == 2 then
		bCheckImpact = true;
		impact_table = self.luxuryData;
	elseif impact_table_number == 3 then
		bCheckImpact = true;
		impact_table = self.bonusData;
	elseif impact_table_number == 4 then
		bCheckImpact = true;
		impact_table = self.fishData;
	elseif impact_table_number == 8 then
		bCheckImpact = true;
		impact_table = self.seaOilData;
	end
	local iW, iH = Map.GetGridSize();
	local iNumLeftToPlace = amount;
	local iNumPlots = table.maxn(plot_list);
	local iNumResources = math.min(amount, math.ceil(ratio * iNumPlots));

	for place_resource = 1, iNumResources do
		for loop, plotIndex in ipairs(plot_list) do
			if bCheckImpact == false or impact_table[plotIndex] == 0 then
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				if res_plot:GetResourceType(-1) == -1 then
					res_plot:SetResourceType(resource_ID, quantity);
					self.amounts_of_resources_placed[resource_ID + 1] = self.amounts_of_resources_placed[resource_ID + 1] + quantity;

					self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
					iNumLeftToPlace = iNumLeftToPlace - 1;
					if bCheckImpact == true then
						local res_addition = 0;
						if max_radius > min_radius then
							res_addition = Map.Rand(1 + (max_radius - min_radius), "Resource Radius - Place Resource LUA");
						end
						local rad = min_radius + res_addition;
						self:PlaceResourceImpact(x, y, impact_table_number, rad)
					end
					break
				end
			end
		end
	end
	return iNumLeftToPlace
end

function AssignStartingPlots:IdentifyRegionsOfThisType(region_type)

	local regions_of_this_type = {};
	for index, current_type in ipairs(self.regionTypes) do
		if current_type == region_type then
			table.insert(regions_of_this_type, index);
		end
	end
	local length = table.maxn(regions_of_this_type);
	if length > 0 then
		local scrambled = GetShuffledCopyOfTable(regions_of_this_type);
		for index, region_to_add in ipairs(scrambled) do
			table.insert(self.regions_sorted_by_type, {region_to_add})
		end
	end
end

function AssignStartingPlots:SortRegionsByType()

	for check_this_type = 1, 9 do
		self:IdentifyRegionsOfThisType(check_this_type)
	end
	self:IdentifyRegionsOfThisType(0)
end

function AssignStartingPlots:AssignLuxuryToRegion(region_number)

	local region_type = self.regionTypes[region_number];
	local luxury_candidates;
	local CoastLux = self.CoastLux;
	local BalancedRegionals = Map.GetCustomOption(14)

	if region_type > 0 and region_type < 9 then
		luxury_candidates = self.luxury_region_weights[region_type];
	else
		luxury_candidates = self.luxury_fallback_weights;
	end


	local iNumAvailableTypes = 0;
	local resource_IDs, resource_weights, res_threshold = {}, {}, {};
	local split_cap = self:GetLuxuriesSplitCap()
	
	for index, resource_options in ipairs(luxury_candidates) do
		local res_ID = resource_options[1];
		if self.luxury_assignment_count[res_ID] < split_cap then
			local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
			if self.iNumTypesAssignedToRegions < self.iNumMaxAllowedForRegions or test == true then

				print("Adding Res ID: " .. res_ID);
				print("Coral ID: " .. tostring(self.coral_ID));


				if res_ID == self.whale_ID or res_ID == self.pearls_ID or res_ID == self.crab_ID or self.bModLuxes and res_ID == self.coral_ID then
					if not self._lek_coastal_refish then
						if self.startLocationConditions[region_number][1] == true then


							local target_list = self:GetRegionLuxuryTargetNumbers()
							local target = target_list[self.iNumCivs]
							local water_needed = 8
							if self.regionTerrainCounts[region_number][8] >= water_needed then
								table.insert(resource_IDs, res_ID);
								local adjusted_weight = resource_options[2] / (0.1 + (self.luxury_assignment_count[res_ID]/2))
								table.insert(resource_weights, adjusted_weight);
								iNumAvailableTypes = iNumAvailableTypes + 1;
							end
						end
					end


				elseif BalancedRegionals == 1 and (res_ID == self.salt_ID or res_ID == self.spices_ID or res_ID == self.gems_ID or res_ID == self.obsidian_ID or res_ID == self.marble_ID or res_ID == self.rubber_ID or res_ID == self.perfume_ID) then

				else
					table.insert(resource_IDs, res_ID);
					local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID])
					table.insert(resource_weights, adjusted_weight);
					iNumAvailableTypes = iNumAvailableTypes + 1;
				end
			end
		end
	end
	

	if iNumAvailableTypes == 0 then
		for index, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < 3 then
				local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
				if self.iNumTypesAssignedToRegions < self.iNumMaxAllowedForRegions or test == true then
					if res_ID == self.whale_ID or res_ID == self.pearls_ID or res_ID == self.crab_ID or self.bModLuxes and res_ID == self.coral_ID then

						if not self._lek_coastal_refish then
							if self.startLocationConditions[region_number][1] == true then


								local target_list = self:GetRegionLuxuryTargetNumbers()
								local target = target_list[self.iNumCivs]
								local water_needed = 8
								if self.regionTerrainCounts[region_number][8] >= water_needed then
									table.insert(resource_IDs, res_ID);
									local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID])
									table.insert(resource_weights, adjusted_weight);
									iNumAvailableTypes = iNumAvailableTypes + 1;
								end
							end
						end
					elseif res_ID == self.salt_ID then

					else
						table.insert(resource_IDs, res_ID);
						local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID])
						table.insert(resource_weights, adjusted_weight);
						iNumAvailableTypes = iNumAvailableTypes + 1;
					end
				end
			end
		end
	end



	if iNumAvailableTypes == 0 then
		print("-"); print("Having to use emergency Luxury assignment process for Region#", region_number);
		print("This likely means a near-maximum number of civs in this game, and problems with not having enough legal Luxury types to spread around.");
		print("If you are modifying luxury types or number of regions allowed to get the same type, check to make sure your changes haven't violated the math so each region can have a legal assignment.");
		for index, resource_options in ipairs(self.luxury_fallback_weights) do
			local res_ID = resource_options[1];
			if self.luxury_assignment_count[res_ID] < 3 then
				local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
				if self.iNumTypesAssignedToRegions < self.iNumMaxAllowedForRegions or test == true then
					table.insert(resource_IDs, res_ID);
					local adjusted_weight = resource_options[2] / (1 + self.luxury_assignment_count[res_ID])
					table.insert(resource_weights, adjusted_weight);
					iNumAvailableTypes = iNumAvailableTypes + 1;
				end
			end
		end
	end
	if iNumAvailableTypes == 0 then
		print("-"); print("FAILED to assign a Luxury type to Region#", region_number); print("-");
	end


	local coast_lux = false;
	local num_coast_lux = 0;
	local totalWeight = 0;
	local coastal_luxes = {};
	for i, this_weight in ipairs(resource_weights) do
		totalWeight = totalWeight + this_weight;
	end
	local accumulatedWeight = 0;
	print("----------------------------------- Regional Luxury Assignment Readout For Region #" .. tostring(region_number) .. "-----------------------------------");
	for index = 1, iNumAvailableTypes do
		local threshold = (resource_weights[index] + accumulatedWeight) * 10000 / totalWeight;
		table.insert(res_threshold, threshold);
		accumulatedWeight = accumulatedWeight + resource_weights[index];
		
		if resource_IDs[index] == 13 or resource_IDs[index] == 14 or resource_IDs[index] == 32 or resource_IDs[index] == 49 then
			coast_lux = true;
			num_coast_lux = num_coast_lux + 1;
			coastal_luxes[resource_IDs[index]] = true;
			table.insert(coastal_luxes, resource_IDs[index]);
		end

		print("Res ID: " .. resource_IDs[index]);
		print("Res Weight: " .. resource_weights[index]);
		print("Threshold: " .. threshold);
	end
	local use_this_ID;

	print("");
	print("");
	print("Coast Start: " .. tostring(self.startLocationConditions[region_number][1]));
	print("Coast Lux: " .. tostring(coast_lux));

	local sea_lux_cahnce = Map.Rand(100, "Chance for sea lux as coastal");

	if sea_lux_cahnce > 0 and CoastLux == false then
		coast_lux = false;
	end

	if self.startLocationConditions[region_number][1] == true and coast_lux == true then
		local diceroll = 1 + Map.Rand(num_coast_lux, "Choose resource type - Assign Luxury To Region - Lua");
		print("----------------------- Coastal Lux Chosen -----------------------");
		print("Num Coastal Luxes: " .. tostring(num_coast_lux));
		print("Diceroll: " .. tostring(diceroll));
		use_this_ID = coastal_luxes[diceroll];
		print("Res ID: " .. tostring(use_this_ID));
	else
		local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To Region - Lua");
		print("Res Diceroll: " .. diceroll);
		for index, threshold in ipairs(res_threshold) do
			if diceroll <= threshold then
				use_this_ID = resource_IDs[index];
				break
			end
		end
	end

	return use_this_ID;
end

function AssignStartingPlots:GetLuxuriesSplitCap()

	local split_cap = 1;


	if self.iNumCivs > 16 then	
		split_cap = 2
	end







	return split_cap
end

function AssignStartingPlots:GetCityStateLuxuriesTargetNumber()

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 4,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 4
		}
	local CSluxCount = worldsizes[Map.GetWorldSize()];
	return CSluxCount
end

function AssignStartingPlots:GetDisabledLuxuriesTargetNumber()


	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = 10,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = 7,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = 0,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = 3,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = 1,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = 0
		}
	local maxToDisable = worldsizes[Map.GetWorldSize()];
	return maxToDisable
end


function AssignStartingPlots:GetRandomLuxuriesTargetNumber()

















	local iW, iH = Map.GetGridSize();

	local LandXY = iW * iH
	local maxRandoms = 30
	local baseLuxCount = 4

	if LandXY < 6700 then
		maxRandoms = (LandXY-720)/((2560-720)/8)+baseLuxCount
	end

	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ LUX COUNT @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
	print("Random Lux Count: " .. maxRandoms);
	print("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

	return maxRandoms
end


function AssignStartingPlots:AssignLuxuryRoles()










	self:SortRegionsByType()


	for index, region_info in ipairs(self.regions_sorted_by_type) do
		local region_number = region_info[1];
		local resource_ID = self:AssignLuxuryToRegion(region_number)
		self.regions_sorted_by_type[index][2] = resource_ID;
		self.region_luxury_assignment[region_number] = resource_ID;
		self.luxury_assignment_count[resource_ID] = self.luxury_assignment_count[resource_ID] + 1;

		print("-"); print("Region#", region_number, " of type ", self.regionTypes[region_number], " has been assigned Luxury ID#", resource_ID);

		local already_assigned = TestMembership(self.resourceIDs_assigned_to_regions, resource_ID)
		if not already_assigned then
			table.insert(self.resourceIDs_assigned_to_regions, resource_ID);
			self.iNumTypesAssignedToRegions = self.iNumTypesAssignedToRegions + 1;


		end
	end
	


	local iNumAvailableTypes = 0;
	local resource_IDs, resource_weights = {}, {};
	for index, resource_options in ipairs(self.luxury_city_state_weights) do
		local res_ID = resource_options[1];
		local test = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
		if test == false then
			table.insert(resource_IDs, res_ID);
			table.insert(resource_weights, resource_options[2]);
			iNumAvailableTypes = iNumAvailableTypes + 1;
		else

		end
	end
	if iNumAvailableTypes < 5 then
		print("---------------------------------------------------------------------------------------");
		print("- Luxuries have been modified in ways disruptive to the City State Assignment Process -");
		print("---------------------------------------------------------------------------------------");
	end

	for cs_lux = 1, 8 do
		local totalWeight = 0;
		local res_threshold = {};
		for i, this_weight in ipairs(resource_weights) do
			totalWeight = totalWeight + this_weight;
		end
		local accumulatedWeight = 0;
		for index, weight in ipairs(resource_weights) do
			local threshold = (weight + accumulatedWeight) * 10000 / totalWeight;
			table.insert(res_threshold, threshold);
			accumulatedWeight = accumulatedWeight + resource_weights[index];
		end
		local use_this_ID;
		local diceroll = Map.Rand(10000, "Choose resource type - City State Luxuries - Lua");
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then
				use_this_ID = resource_IDs[index];
				table.insert(self.resourceIDs_assigned_to_cs, use_this_ID);
				table.remove(resource_IDs, index);
				table.remove(resource_weights, index);



				break
			end
		end
	end
	




	

	



	




	

	


	local remaining_resource_IDs, rand_resource_IDs, rand_resource_weights = {}, {}, {}
	for index, resource_options in ipairs(self.luxury_fallback_weights) do
		local res_ID = resource_options[1]
		local test1 = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
		local test2 = TestMembership(self.resourceIDs_assigned_to_cs, res_ID)
		if test1 == false and test2 == false then
			table.insert(rand_resource_IDs, res_ID)
			table.insert(rand_resource_weights, resource_options[2])
		else

		end
	end	
	
	self.iNumTypesRandom = self:GetRandomLuxuriesTargetNumber()	
	for rand_lux = 1, self.iNumTypesRandom do
		local totalWeight = 0
		local res_threshold = {}
		for i, this_weight in ipairs(rand_resource_weights) do
			totalWeight = totalWeight + this_weight
		end
		local accumulatedWeight = 0
		for index, weight in ipairs(rand_resource_weights) do
			local threshold = (weight + accumulatedWeight) * 10000 / totalWeight
			table.insert(res_threshold, threshold)
			accumulatedWeight = accumulatedWeight + rand_resource_weights[index]
		end
		local use_this_ID
		local diceroll = Map.Rand(10000, "Choose resource type - Random Luxuries - Lua")
		for index, threshold in ipairs(res_threshold) do
			if diceroll < threshold then
				use_this_ID = rand_resource_IDs[index]
				table.insert(self.resourceIDs_assigned_to_random, use_this_ID)
				table.remove(rand_resource_IDs, index)
				table.remove(rand_resource_weights, index)
				print("-") print("Luxury ID#", use_this_ID, "assigned to Random.")
				break
			end
		end
	end
	

	for index, resource_options in ipairs(self.luxury_fallback_weights) do
		local res_ID = resource_options[1]

		local test1 = TestMembership(self.resourceIDs_assigned_to_regions, res_ID)
		local test2 = TestMembership(self.resourceIDs_assigned_to_cs, res_ID)
		local test3 = TestMembership(self.resourceIDs_assigned_to_random, res_ID)
		if test1 == false and test2 == false and test3 == false then
			table.insert(self.resourceIDs_not_being_used, res_ID)
			print("-") print("Luxury ID#", res_ID, "disabled.")
		else

		end
	end

	


























	

	print("--- Luxury Assignment Table ---");
	print("-"); print("- - Assigned to Regions - -");
	for index, data in ipairs(self.regions_sorted_by_type) do
		print("Region#", data[1], "has Luxury type", data[2]);
	end
	print("-"); print("Total unique regional luxuries: ", self.iNumTypesAssignedToRegions);
	print("-"); print("- - Assigned to City States - -");
	for index, type in ipairs(self.resourceIDs_assigned_to_cs) do
		print("Luxury type", type);
	end
	print("-"); print("- - Assigned to Random - -");
	for index, type in ipairs(self.resourceIDs_assigned_to_random) do
		print("Luxury type", type);
	end
	print("-"); print("- - Luxuries handled via Special Case - -");
	for index, type in ipairs(self.resourceIDs_assigned_to_special_case) do
		print("Luxury type", type);
	end
	print("-"); print("- - Disabled - -");
	for index, type in ipairs(self.resourceIDs_not_being_used) do
		print("Luxury type", type);
	end
	print("- - - - - - - - - - - - - - - -");

end

function AssignStartingPlots:GetListOfAllowableLuxuriesAtCitySite(x, y, radius, loc_is_coastal)

	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local allowed_luxuries = table.fill(false, 99);
	
	for ripple_radius = 1, radius do
		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;




					if plot:GetResourceType(-1) == -1 then	



						if plotType == PlotTypes.PLOT_OCEAN then
							if terrainType == TerrainTypes.TERRAIN_COAST then
								if plot:IsLake() == false then
									if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE and loc_is_coastal == true then
										allowed_luxuries[self.whale_ID] = true
										allowed_luxuries[self.pearls_ID] = true
										allowed_luxuries[self.crab_ID] = true
										if self.bModLuxes == true then
											allowed_luxuries[self.coral_ID] = true
										end
									end
								end
							end



						elseif plotType == PlotTypes.PLOT_HILLS or plotType == PlotTypes.PLOT_LAND then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA then
								allowed_luxuries[self.marble_ID] = true
								allowed_luxuries[self.gold_ID] = true
								allowed_luxuries[self.silver_ID] = true
								allowed_luxuries[self.copper_ID] = true	
								allowed_luxuries[self.gems_ID] = true
								allowed_luxuries[self.salt_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.jade_ID] = true
									allowed_luxuries[self.amber_ID] = true
									allowed_luxuries[self.lapis_ID] = true
									allowed_luxuries[self.obsidian_ID] = true
								end

								allowed_luxuries[self.fur_ID] = true
								allowed_luxuries[self.dye_ID] = true
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								allowed_luxuries[self.marble_ID] = true
								allowed_luxuries[self.gold_ID] = true
								allowed_luxuries[self.silver_ID] = true
								allowed_luxuries[self.copper_ID] = true	
								allowed_luxuries[self.gems_ID] = true
								allowed_luxuries[self.salt_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.jade_ID] = true
									allowed_luxuries[self.amber_ID] = true
									allowed_luxuries[self.lapis_ID] = true
									allowed_luxuries[self.obsidian_ID] = true
								end

								allowed_luxuries[self.incense_ID] = true
								allowed_luxuries[self.ivory_ID] = true
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								allowed_luxuries[self.marble_ID] = true
								allowed_luxuries[self.gold_ID] = true
								allowed_luxuries[self.silver_ID] = true
								allowed_luxuries[self.copper_ID] = true	
								allowed_luxuries[self.gems_ID] = true
								allowed_luxuries[self.salt_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.jade_ID] = true
									allowed_luxuries[self.amber_ID] = true
									allowed_luxuries[self.lapis_ID] = true
									allowed_luxuries[self.obsidian_ID] = true
								end


								allowed_luxuries[self.spices_ID] = true
								allowed_luxuries[self.silk_ID] = true
								allowed_luxuries[self.sugar_ID] = true
								allowed_luxuries[self.citrus_ID] = true
								allowed_luxuries[self.truffles_ID] = true
								allowed_luxuries[self.cocoa_ID] = true
								allowed_luxuries[self.fur_ID] = true
								allowed_luxuries[self.dye_ID] = true
								allowed_luxuries[self.coconut_ID] = true
								allowed_luxuries[self.rubber_ID] = true

								allowed_luxuries[self.cotton_ID] = true
								allowed_luxuries[self.wine_ID] = true
								allowed_luxuries[self.ivory_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.coffee_ID] = true
									allowed_luxuries[self.tea_ID] = true
									allowed_luxuries[self.tobacco_ID] = true
									allowed_luxuries[self.perfume_ID] = true
									allowed_luxuries[self.olives_ID] = true
									
								end

								allowed_luxuries[self.incense_ID] = true
							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								allowed_luxuries[self.marble_ID] = true
								allowed_luxuries[self.gold_ID] = true
								allowed_luxuries[self.silver_ID] = true
								allowed_luxuries[self.copper_ID] = true	
								allowed_luxuries[self.gems_ID] = true
								allowed_luxuries[self.salt_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.jade_ID] = true
									allowed_luxuries[self.amber_ID] = true
									allowed_luxuries[self.lapis_ID] = true
									allowed_luxuries[self.obsidian_ID] = true
								end


								allowed_luxuries[self.spices_ID] = true
								allowed_luxuries[self.silk_ID] = true
								allowed_luxuries[self.sugar_ID] = true
								allowed_luxuries[self.citrus_ID] = true
								allowed_luxuries[self.truffles_ID] = true
								allowed_luxuries[self.cocoa_ID] = true
								allowed_luxuries[self.fur_ID] = true
								allowed_luxuries[self.dye_ID] = true
								allowed_luxuries[self.coconut_ID] = true

								allowed_luxuries[self.cotton_ID] = true
								allowed_luxuries[self.wine_ID] = true
								allowed_luxuries[self.ivory_ID] = true
								
								if self.bModLuxes == true then
									allowed_luxuries[self.coffee_ID] = true
									allowed_luxuries[self.tea_ID] = true
									allowed_luxuries[self.tobacco_ID] = true
									allowed_luxuries[self.olives_ID] = true
									allowed_luxuries[self.rubber_ID] = true
								end

								allowed_luxuries[self.incense_ID] = true
							end
						end
					end

					currentX, currentY = nextX, nextY;
				end
			end
		end
	end
	return allowed_luxuries
end

function AssignStartingPlots:GenerateLuxuryPlotListsAtCitySite(x, y, radius, bRemoveFeatureIce)




	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local region_coast, region_marsh, region_flood_plains, region_tundra_flat_including_forests = {}, {}, {}, {};
	local region_hills_open, region_hills_covered, region_hills_jungle, region_hills_forest = {}, {}, {}, {};
	local region_desert_flat_no_feature, region_plains_flat_no_feature, region_jungle_flat = {}, {}, {};
	local region_forest_flat, region_forest_flat_but_not_tundra = {}, {};
	local region_dry_grass_flat_no_feature, region_fresh_water_grass_flat_no_feature = {}, {};
	local region_dry_plains_flat_no_feature, region_fresh_water_plains_flat_no_feature = {}, {};
	local region_desert_or_tundra_flat_no_feature, region_grass_flat_no_feature = {}, {};
	local region_tundra_flat_no_feature, region_tundra_flat_forest = {}, {};
	local region_hills_open_no_tundra, region_hills_open_no_desert, region_hills_open_no_tundra_no_desert = {}, {}, {};
	local region_hills_open_no_grass, region_hills_open_no_grass_no_tundra, region_hills_covered_no_tundra = {}, {}, {};
	local region_hills_covered_no_grass, region_hills_covered_no_grass_no_tundra, region_flat_covered = {}, {}, {};
	local region_flat_covered_no_grass, region_flat_covered_no_tundra, region_flat_covered_no_grass_no_tundra = {}, {}, {};
	local region_flat_open, region_flat_open_no_grass_no_plains, region_flat_open_no_tundra_no_desert = {}, {}, {};
	local region_flat_open_no_desert, region_flat_desert_including_flood, region_hills_open_no_grass_no_plains = {}, {}, {};



	if radius > 0 and radius < 6 then
		for ripple_radius = 1, radius do
			local ripple_value = radius - ripple_radius + 1;
			local currentX = x - ripple_radius;
			local currentY = y;
			for direction_index = 1, 6 do
				for plot_to_handle = 1, ripple_radius do
				 	if currentY / 2 > math.floor(currentY / 2) then
						plot_adjustments = odd[direction_index];
					else
						plot_adjustments = even[direction_index];
					end
					nextX = currentX + plot_adjustments[1];
					nextY = currentY + plot_adjustments[2];
					if wrapX == false and (nextX < 0 or nextX >= iW) then

					elseif wrapY == false and (nextY < 0 or nextY >= iH) then

					else
						local realX = nextX;
						local realY = nextY;
						if wrapX then
							realX = realX % iW;
						end
						if wrapY then
							realY = realY % iH;
						end

						local plot = Map.GetPlot(realX, realY);
						
						local plotType = plot:GetPlotType()
						local terrainType = plot:GetTerrainType()
						local featureType = plot:GetFeatureType()
						local plotIndex = realY * iW + realX + 1;
						

						if bRemoveFeatureIce == true then
							if featureType == FeatureTypes.FEATURE_ICE then
								plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
							end
							

						else
							if plotType == PlotTypes.PLOT_OCEAN then
								if terrainType == TerrainTypes.TERRAIN_COAST then
									if plot:IsLake() == false then
										if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
											table.insert(region_coast, plotIndex);
										end
									end
								end
							elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
								if featureType == FeatureTypes.NO_FEATURE then
									table.insert(region_hills_open, plotIndex);
									if terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(region_hills_open_no_desert, plotIndex);
										table.insert(region_hills_open_no_grass, plotIndex);
										table.insert(region_hills_open_no_grass_no_plains, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_DESERT then
										table.insert(region_hills_open_no_tundra, plotIndex);
										table.insert(region_hills_open_no_grass, plotIndex);
										table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
										table.insert(region_hills_open_no_grass_no_plains, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_hills_open_no_tundra, plotIndex);
										table.insert(region_hills_open_no_desert, plotIndex);
										table.insert(region_hills_open_no_grass, plotIndex);
										table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
										table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_hills_open_no_tundra, plotIndex);
										table.insert(region_hills_open_no_desert, plotIndex);
										table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
									end
								elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
									table.insert(region_hills_jungle, plotIndex);
									table.insert(region_hills_covered, plotIndex);
									if terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_hills_covered_no_tundra, plotIndex);
										table.insert(region_hills_covered_no_grass, plotIndex);
										table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_hills_covered_no_tundra, plotIndex);
									end
								elseif featureType == FeatureTypes.FEATURE_FOREST then		
									table.insert(region_hills_forest, plotIndex);
									table.insert(region_hills_covered, plotIndex);
									if terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(region_hills_covered_no_grass, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_hills_covered_no_tundra, plotIndex);
										table.insert(region_hills_covered_no_grass, plotIndex);
										table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_hills_covered_no_tundra, plotIndex);
									end
								end
							elseif plotType == PlotTypes.PLOT_LAND then
								if featureType == FeatureTypes.NO_FEATURE then
									if terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(region_tundra_flat_no_feature, plotIndex);
										table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
										table.insert(region_tundra_flat_including_forests, plotIndex);
										table.insert(region_flat_open, plotIndex);
										table.insert(region_flat_open_no_desert, plotIndex);
										table.insert(region_flat_open_no_grass_no_plains, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_DESERT then
										table.insert(region_desert_flat_no_feature, plotIndex);
										table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
										table.insert(region_flat_open, plotIndex);
										table.insert(region_flat_open_no_grass_no_plains, plotIndex);
										table.insert(region_flat_desert_including_flood, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_plains_flat_no_feature, plotIndex);
										table.insert(region_flat_open_no_desert, plotIndex);
										table.insert(region_flat_open, plotIndex);
										table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
										if plot:IsFreshWater() then
											table.insert(region_fresh_water_plains_flat_no_feature, plotIndex);
										else
											table.insert(region_dry_plains_flat_no_feature, plotIndex);
										end
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_grass_flat_no_feature, plotIndex);
										table.insert(region_flat_open_no_desert, plotIndex);
										table.insert(region_flat_open, plotIndex);
										table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
										if plot:IsFreshWater() then
											table.insert(region_fresh_water_grass_flat_no_feature, plotIndex);
										else
											table.insert(region_dry_grass_flat_no_feature, plotIndex);
										end
									end
								elseif featureType == FeatureTypes.FEATURE_MARSH then		
									table.insert(region_marsh, plotIndex);
								elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
									table.insert(region_flood_plains, plotIndex);
									table.insert(region_flat_desert_including_flood, plotIndex);
								elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
									table.insert(region_jungle_flat, plotIndex);
									if terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_flat_covered_no_tundra, plotIndex);
										table.insert(region_flat_covered_no_grass, plotIndex);
										table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_flat_covered_no_tundra, plotIndex);
									end
								elseif featureType == FeatureTypes.FEATURE_FOREST then		
									table.insert(region_forest_flat, plotIndex);
									if terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(region_tundra_flat_including_forests, plotIndex);

										table.insert(region_tundra_flat_forest, plotIndex);
										table.insert(region_flat_covered_no_grass, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(region_forest_flat_but_not_tundra, plotIndex);
										table.insert(region_flat_covered_no_tundra, plotIndex);
										table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
										table.insert(region_flat_covered_no_grass, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(region_forest_flat_but_not_tundra, plotIndex);
										table.insert(region_flat_covered_no_tundra, plotIndex);
									end
								end
							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end
			
	local results_table = {
	region_coast,
	region_marsh,
	region_flood_plains,
	region_hills_open,
	region_hills_covered,
	region_hills_jungle,
	region_hills_forest,
	region_jungle_flat,
	region_forest_flat,
	region_desert_flat_no_feature,
	region_plains_flat_no_feature,
	region_dry_grass_flat_no_feature,
	region_fresh_water_grass_flat_no_feature,
	region_tundra_flat_including_forests,
	region_forest_flat_but_not_tundra,
	region_grass_flat_no_feature,
	region_tundra_flat_no_feature,
	region_dry_plains_flat_no_feature,
	region_fresh_water_plains_flat_no_feature,
	region_desert_or_tundra_flat_no_feature,
	region_tundra_flat_forest,
	region_hills_open_no_tundra,
	region_hills_open_no_desert,
	region_hills_open_no_tundra_no_desert,
	region_hills_open_no_grass,
	region_hills_open_no_grass_no_tundra,
	region_hills_open_no_grass_no_plains,
	region_hills_covered_no_tundra,
	region_hills_covered_no_grass,
	region_hills_covered_no_grass_no_tundra,
	region_flat_covered,
	region_flat_covered_no_grass,
	region_flat_covered_no_tundra,
	region_flat_covered_no_grass_no_tundra,
	region_flat_open,
	region_flat_open_no_grass_no_plains,
	region_flat_open_no_tundra_no_desert,
	region_flat_open_no_desert,
	region_flat_desert_including_flood,
	};
	return results_table
end

function AssignStartingPlots:GenerateLuxuryPlotListsInRegion(region_number)
	local iW, iH = Map.GetGridSize();

	local region_data_table = self.regionData[region_number];
	local iWestX = region_data_table[1];
	local iSouthY = region_data_table[2];
	local iWidth = region_data_table[3];
	local iHeight = region_data_table[4];
	local iAreaID = region_data_table[5];
	local region_area_object;
	if iAreaID ~= -1 then
		region_area_object = Map.GetArea(iAreaID);
	end

	local region_coast, region_marsh, region_flood_plains, region_tundra_flat_including_forests = {}, {}, {}, {};
	local region_hills_open, region_hills_covered, region_hills_jungle, region_hills_forest = {}, {}, {}, {};
	local region_desert_flat_no_feature, region_plains_flat_no_feature, region_jungle_flat = {}, {}, {};
	local region_forest_flat, region_forest_flat_but_not_tundra = {}, {};
	local region_dry_grass_flat_no_feature, region_fresh_water_grass_flat_no_feature = {}, {};
	local region_dry_plains_flat_no_feature, region_fresh_water_plains_flat_no_feature = {}, {};
	local region_desert_or_tundra_flat_no_feature, region_grass_flat_no_feature = {}, {};
	local region_tundra_flat_no_feature, region_tundra_flat_forest = {}, {};
	local region_hills_open_no_tundra, region_hills_open_no_desert, region_hills_open_no_tundra_no_desert = {}, {}, {};
	local region_hills_open_no_grass, region_hills_open_no_grass_no_tundra, region_hills_covered_no_tundra = {}, {}, {};
	local region_hills_covered_no_grass, region_hills_covered_no_grass_no_tundra, region_flat_covered = {}, {}, {};
	local region_flat_covered_no_grass, region_flat_covered_no_tundra, region_flat_covered_no_grass_no_tundra = {}, {}, {};
	local region_flat_open, region_flat_open_no_grass_no_plains, region_flat_open_no_tundra_no_desert = {}, {}, {};
	local region_flat_open_no_desert, region_flat_desert_including_flood, region_hills_open_no_grass_no_plains = {}, {}, {};


	for region_loop_y = 0, iHeight - 1 do
		for region_loop_x = 0, iWidth - 1 do
			local x = (region_loop_x + iWestX) % iW;
			local y = (region_loop_y + iSouthY) % iH;
			local plotIndex = y * iW + x + 1;
			local plot = Map.GetPlot(x, y);
			local area_of_plot = plot:GetArea();

			local plotType = plot:GetPlotType()
			local terrainType = plot:GetTerrainType()
			local featureType = plot:GetFeatureType()

			if plotType == PlotTypes.PLOT_OCEAN then
				if terrainType == TerrainTypes.TERRAIN_COAST then
					if plot:IsLake() == false then
						if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
							if iAreaID == -1 then
								if plot:IsAdjacentToLand() then
									table.insert(region_coast, plotIndex);
								end
							else
								if plot:IsAdjacentToArea(region_area_object) then
									table.insert(region_coast, plotIndex);
								end
							end
						end
					end
				end
			elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
				if featureType == FeatureTypes.NO_FEATURE then
					table.insert(region_hills_open, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
						table.insert(region_hills_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_grass, plotIndex);
						table.insert(region_hills_open_no_grass_no_tundra, plotIndex);
						table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_open_no_tundra, plotIndex);
						table.insert(region_hills_open_no_desert, plotIndex);
						table.insert(region_hills_open_no_tundra_no_desert, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
					table.insert(region_hills_jungle, plotIndex);
					table.insert(region_hills_covered, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
						table.insert(region_hills_covered_no_grass, plotIndex);
						table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_FOREST then		
					table.insert(region_hills_forest, plotIndex);
					table.insert(region_hills_covered, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_hills_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
						table.insert(region_hills_covered_no_grass, plotIndex);
						table.insert(region_hills_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_hills_covered_no_tundra, plotIndex);
					end
				end
			elseif plotType == PlotTypes.PLOT_LAND then
				if featureType == FeatureTypes.NO_FEATURE then
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_tundra_flat_no_feature, plotIndex);
						table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
						table.insert(region_tundra_flat_including_forests, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open_no_grass_no_plains, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_DESERT then
						table.insert(region_desert_flat_no_feature, plotIndex);
						table.insert(region_desert_or_tundra_flat_no_feature, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_grass_no_plains, plotIndex);
						table.insert(region_flat_desert_including_flood, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_plains_flat_no_feature, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
						if plot:IsFreshWater() then
							table.insert(region_fresh_water_plains_flat_no_feature, plotIndex);
						else
							table.insert(region_dry_plains_flat_no_feature, plotIndex);
						end
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_grass_flat_no_feature, plotIndex);
						table.insert(region_flat_open_no_desert, plotIndex);
						table.insert(region_flat_open, plotIndex);
						table.insert(region_flat_open_no_tundra_no_desert, plotIndex);
						if plot:IsFreshWater() then
							table.insert(region_fresh_water_grass_flat_no_feature, plotIndex);
						else
							table.insert(region_dry_grass_flat_no_feature, plotIndex);
						end
					end
				elseif featureType == FeatureTypes.FEATURE_MARSH then		
					table.insert(region_marsh, plotIndex);
				elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
					table.insert(region_flood_plains, plotIndex);
					table.insert(region_flat_desert_including_flood, plotIndex);
				elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
					table.insert(region_jungle_flat, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_flat_covered_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
						table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_flat_covered_no_tundra, plotIndex);
					end
				elseif featureType == FeatureTypes.FEATURE_FOREST then		
					table.insert(region_forest_flat, plotIndex);
					if terrainType == TerrainTypes.TERRAIN_TUNDRA then
						table.insert(region_tundra_flat_including_forests, plotIndex);

						table.insert(region_tundra_flat_forest, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
						table.insert(region_forest_flat_but_not_tundra, plotIndex);
						table.insert(region_flat_covered_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass_no_tundra, plotIndex);
						table.insert(region_flat_covered_no_grass, plotIndex);
					elseif terrainType == TerrainTypes.TERRAIN_GRASS then
						table.insert(region_forest_flat_but_not_tundra, plotIndex);
						table.insert(region_flat_covered_no_tundra, plotIndex);
					end
				end
			end
		end
	end
	

	local results_table = {
	region_coast,
	region_marsh,
	region_flood_plains,
	region_hills_open,
	region_hills_covered,
	region_hills_jungle,
	region_hills_forest,
	region_jungle_flat,
	region_forest_flat,
	region_desert_flat_no_feature,
	region_plains_flat_no_feature,
	region_dry_grass_flat_no_feature,
	region_fresh_water_grass_flat_no_feature,
	region_tundra_flat_including_forests,
	region_forest_flat_but_not_tundra,
	region_grass_flat_no_feature,
	region_tundra_flat_no_feature,
	region_dry_plains_flat_no_feature,
	region_fresh_water_plains_flat_no_feature,
	region_desert_or_tundra_flat_no_feature,
	region_tundra_flat_forest,
	region_hills_open_no_tundra,
	region_hills_open_no_desert,
	region_hills_open_no_tundra_no_desert,
	region_hills_open_no_grass,
	region_hills_open_no_grass_no_tundra,
	region_hills_open_no_grass_no_plains,
	region_hills_covered_no_tundra,
	region_hills_covered_no_grass,
	region_hills_covered_no_grass_no_tundra,
	region_flat_covered,
	region_flat_covered_no_grass,
	region_flat_covered_no_tundra,
	region_flat_covered_no_grass_no_tundra,
	region_flat_open,
	region_flat_open_no_grass_no_plains,
	region_flat_open_no_tundra_no_desert,
	region_flat_open_no_desert,
	region_flat_desert_including_flood,
	};
	return results_table
end

function AssignStartingPlots:GetIndicesForLuxuryType(resource_ID)





	local primary, secondary, tertiary, quaternary, quinary, senary = -1, -1, -1, -1, -1, -1;



	if resource_ID == self.whale_ID then
		primary = 1;
	elseif resource_ID == self.pearls_ID then
		primary = 1;
	elseif resource_ID == self.crab_ID then
		primary = 1;
	elseif resource_ID == self.coral_ID then
		primary = 1;

	elseif resource_ID == self.marble_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.gold_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.silver_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.copper_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.gems_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.salt_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.jade_ID then							
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.amber_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;	
	elseif resource_ID == self.lapis_ID then							
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;
	elseif resource_ID == self.obsidian_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 27, 24, 36, 37, 5, 31;

	elseif resource_ID == self.cocoa_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.citrus_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.spices_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.sugar_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.truffles_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 15, 8, 28, 37, 24, 2;
	elseif resource_ID == self.rubber_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.coconut_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 8, 15, 28, 37, 24, 2;
	elseif resource_ID == self.silk_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 15, 8, 28, 37, 24, 2;
	elseif resource_ID == self.dye_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 9, 8, 5, 38, 23, 2;
	elseif resource_ID == self.fur_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 21, 15, 7, 38, 23, 2;

	elseif resource_ID == self.incense_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 39, 11, 22, 33, 28, 16;
	elseif resource_ID == self.ivory_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 11, 16, 39, 22, 33, 28;
	elseif resource_ID == self.wine_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 11, 16, 33, 24, 28, 2;
	elseif resource_ID == self.olives_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 11, 16, 33, 24, 28, 2;
	elseif resource_ID == self.coffee_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 11, 16, 33, 24, 28, 2;	
	elseif resource_ID == self.tobacco_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 16, 11, 33, 24, 28, 2;
	elseif resource_ID == self.tea_ID then							
		primary, secondary, tertiary, quaternary, quinary, senary = 16, 11, 33, 24, 28, 2;
	elseif resource_ID == self.perfume_ID then						
		primary, secondary, tertiary, quaternary, quinary, senary = 16, 11, 33, 24, 28, 2;
	elseif resource_ID == self.cotton_ID then
		primary, secondary, tertiary, quaternary, quinary, senary = 16, 11, 33, 24, 28, 2;
	end


	return primary, secondary, tertiary, quaternary, quinary, senary;
end

function AssignStartingPlots:GetRegionLuxuryTargetNumbers()






	local duel_values = table.fill(1, 22);













	local tiny_values = {0, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	local small_values = {0, 3, 3, 3, 4, 4, 4, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

	local standard_values = {0, 3, 3, 4, 4, 5, 5, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1};

	local large_values = {0, 3, 4, 4, 5, 5, 5, 6, 6, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 2, 2};

	local huge_values = {0, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3};

	local worldsizes = {
		[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = duel_values,
		[GameInfo.Worlds.WORLDSIZE_TINY.ID] = tiny_values,
		[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = small_values,
		[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = standard_values,
		[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = large_values,
		[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = huge_values
		}
	local target_list = worldsizes[Map.GetWorldSize()];
	return target_list
end

function AssignStartingPlots:GetWorldLuxuryTargetNumbers()







	local worldsizes = {};
	if self.resource_setting == 1 or self.resource_setting == 2 or self.resource_setting == 3 then
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {14, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {24, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {36, 4},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {48, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {76, 6}
		}
	elseif self.resource_setting == 7 or self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {24, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {40, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {80, 5},
			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {80, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {100, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {128, 6}
		}
	else
		worldsizes = {
			[GameInfo.Worlds.WORLDSIZE_DUEL.ID] = {20, 3},
			[GameInfo.Worlds.WORLDSIZE_TINY.ID] = {35, 4},
			[GameInfo.Worlds.WORLDSIZE_SMALL.ID] = {60, 5},

			[GameInfo.Worlds.WORLDSIZE_STANDARD.ID] = {60, 5},
			[GameInfo.Worlds.WORLDSIZE_LARGE.ID] = {88, 5},
			[GameInfo.Worlds.WORLDSIZE_HUGE.ID] = {112, 6}
		}
	end
	local world_size_data = worldsizes[Map.GetWorldSize()];
	return world_size_data
end


function AssignStartingPlots:PlaceLuxuries()

	local iW, iH = Map.GetGridSize();

	local used_randoms_as_secondaries =	table.fill(false, 99);

	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);

		local iNumToPlace = 2;
		if self.start_locations == 1 or self.start_locations == 2 then
			iNumToPlace = 3;
		end
















		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(this_region_luxury);


		luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumToPlace, 0.5, -1, 0, 0, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 0.5, -1, 0, 0, shuf_list);
		end

		if iNumLeftToPlace > 0 then

			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
		end

		if iNumLeftToPlace > 0 then



			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] - iNumLeftToPlace;
		end

		if iNumLeftToPlace > 0 and self.iNumTypesRandom > 0 then

			local randoms_to_place = 1;
			for loop, random_res in ipairs(self.resourceIDs_assigned_to_random) do

				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(random_res);
				if randoms_to_place > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if randoms_to_place > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					randoms_to_place = self:PlaceSpecificNumberOfResources(random_res, 1, 1, 1, -1, 0, 0, shuf_list);
				end
			end
		end
	end
	


	for city_state = 1, self.iNumCityStates do

		if self.city_state_validity_table[city_state] == false then

		else
			

			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			local lux_possible_for_cs = {};


			local cs_only_types = {};
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] == true then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for loop, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 80 / iNumCSAllowed;
				end
			end


			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[res_ID] == true then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;










				if iNumRandAllowed > 0 then
					for loop, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 20 / iNumAllowed;
					end
				end
			end


			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else

				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end


				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");

				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					if diceroll < threshold then
						use_this_ID = res_ID;
						print("CS Given Lux ID: " .. tostring(use_this_ID));
						break
					end
					accumulatedWeight = accumulatedWeight + this_weight;
				end

				print("-"); print("-"); print("-Assigned Luxury Type", use_this_ID, "to City State#", city_state);

				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
                primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
				end
			end
		end
	end
		

	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local iNumAlreadyPlaced = self.amounts_of_resources_placed[res_ID + 1];
		local assignment_split = self.luxury_assignment_count[res_ID];
		local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list, iNumLeftToPlace;
		primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);
		luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number)




		local target_list = self:GetRegionLuxuryTargetNumbers()
		local targetNum = target_list[self.iNumCivs]

		targetNum = targetNum - self.region_low_fert_compensation[region_number];

		if self.resource_setting == 1 or self.resource_setting == 2 then
			targetNum = targetNum - 2;
		elseif self.resource_setting == 3 or self.resource_setting == 4 or self.resource_setting == 5 or self.resource_setting == 6 then
			targetNum = targetNum - 1;
		elseif self.resource_setting == 7 then
			targetNum = targetNum + 1;
		elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
			targetNum = targetNum + 2;
		end
		local iNumThisLuxToPlace = math.max(1, targetNum);

		print("-"); print("Target number for Luxury#", res_ID, "with assignment split of", assignment_split, "is", targetNum);
		

		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, 1, 3, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 0, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and quinary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and senary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.25, 2, 1, 3, shuf_list);
		end
		print("-"); print("-"); print("Number of LuxuryID", res_ID, "left to place in Region#", region_number, "is", iNumLeftToPlace);
		



		if iNumLeftToPlace > 0 then	

			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 1, 2, 1, 3, shuf_list);
			end
			print("-"); print("Number of LuxuryID", res_ID, "not placed in Region#", region_number, "is", iNumLeftToPlace);
		end	
	end


	if self.iNumTypesRandom > 0 then
		print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");





		local world_size_data = self:GetWorldLuxuryTargetNumbers()
		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		print("* *"); print("* targetLuxForThisWorldSize = ", targetLuxForThisWorldSize); print("* *");
		print("* *"); print("* random to add to target = ", extraLux); print("* *");
		print("* *"); print("* totalLuxPlacedSoFar = ", self.totalLuxPlacedSoFar); print("* *");
		print("* *"); print("* iNumRandomLuxTarget = ", iNumRandomLuxTarget); print("* *");
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;

		local random_lux_ratios_table = {
		{1},
		{1, 1},
		{1, 1, 1},
		{1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1} };

		for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
			local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, current_list, iNumLeftToPlace;
			primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(res_ID);

				local iW, iH = Map.GetGridSize();
				local LandXY = iW * iH;
				local NumRandToAdd = 4;

				if LandXY < 2500 then
					NumRandToAdd = 4;
				elseif LandXY < 6000 then
					NumRandToAdd = 5;
				elseif LandXY < 10000 then
					NumRandToAdd = 6;
				end

				iNumThisLuxToPlace = math.max(NumRandToAdd, math.ceil(iNumRandomLuxTarget / 10));






			local lux_distance = 3;


			current_list = self.global_luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.25, 2, lux_distance, 0, current_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				current_list = self.global_luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.3, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				current_list = self.global_luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.35, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				current_list = self.global_luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.4, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and quinary > 0 then
				current_list = self.global_luxury_plot_lists[quinary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.5, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and senary > 0 then
				current_list = self.global_luxury_plot_lists[senary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.75, 2, lux_distance, 0, current_list);
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			print("-"); 
			print("Random Luxury ID#:", res_ID);
			print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
		end
		print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
	end




	local placed2ndLux = true;

	if self.resource_setting ~= 1 then

		print("|||||||||||||||||||||||||||||||||||| Secondary Lux Check ||||||||||||||||||||||||||||||||||||");

		local coastal_rotation = 1;
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];

			local cplot = Map.GetPlot(x, y)

			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");

			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if (self.start_locations ~= 1) and (self.start_locations ~= 2) and (self.start_locations ~= 3) then
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end


			if self._lek_coastal_refish and Map.GetPlot(x, y):IsCoastalLand() then
				iNumTypesAllowed = 0;
				candidate_types = {};
				table.insert(candidate_types, self.whale_ID);
				table.insert(candidate_types, self.pearls_ID);
				table.insert(candidate_types, self.crab_ID);
				table.insert(candidate_types, self.coral_ID);

				local force_id = coastal_rotation % 4
				coastal_rotation = coastal_rotation + 1
				use_this_ID = candidate_types[force_id];

				print("sapht: forcing a coastal lux rot/id = ", force_id, use_this_ID)
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");

				iNumTypesAllowed = iNumTypesAllowed + 1;


			elseif iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				print("sapht: rolling lux dice")
				use_this_ID = candidate_types[diceroll];
			else

				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else

					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
								print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						print("-"); print("Failed to place second Luxury type in 2nd ring at start in Region#", region_number, "-- no eligible types!"); print("-");
						placed2ndLux = false;
					end
				end
			end
			print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");
			print("Random Res 2 Rings: " .. tostring(use_this_ID));

			if use_this_ID ~= nil then
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
					used_randoms_as_secondaries[use_this_ID] = true;
					print("Random Res State: " .. tostring(used_randoms_as_secondaries[use_this_ID]));
				end
			end
		end
	end


	if placed2ndLux == false then
		
		local placed2ndLux = true;

		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 3)
			print("-"); print("--- Eligible Types List for Second Luxury in Region#", region_number, "---");

			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
					print("- Found eligible luxury type:", res_ID);
					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if (self.start_locations ~= 1) and (self.start_locations ~= 2) and (self.start_locations ~= 3) then
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end


			if self._lek_coastal_refish and Map.GetPlot(x, y):IsCoastalLand() then
				iNumTypesAllowed = 0;
				candidate_types = {};
				table.insert(candidate_types, self.whale_ID);
				table.insert(candidate_types, self.pearls_ID);
				table.insert(candidate_types, self.crab_ID);
				table.insert(candidate_types, self.coral_ID);
				iNumTypesAllowed = iNumTypesAllowed + 4;
				print("sapht: forcing a coastal lux (2)")


			end
		
			if iNumTypesAllowed > 0 then
				local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
				use_this_ID = candidate_types[diceroll];
			else

				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
				if iNumTypesAllowed > 0 then
					local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
					use_this_ID = candidate_types[diceroll];
				else

					local region_lux_ID = self.region_luxury_assignment[region_number];
					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
						if res_ID ~= region_lux_ID then
							if allowed_luxuries[res_ID] == true and used_randoms_as_secondaries[res_ID] == false then
								print("- Found eligible luxury type:", res_ID);
								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else
						print("-"); print("Failed to place second Luxury type in 3rd ring at start in Region#", region_number, "-- no eligible types!"); print("-");
						placed2ndLux = false;
					end
				end
			end
			print("--- End of Eligible Types list for Second Luxury in Region#", region_number, "---");

			print("Random Res 3 Rings: " .. tostring(use_this_ID));

			if use_this_ID ~= nil then
				local primary, secondary, tertiary, quaternary, quinary, senary, luxury_plot_lists, shuf_list;
				primary, secondary, tertiary, quaternary, quinary, senary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and senary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[senary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
					used_randoms_as_secondaries[use_this_ID] = true;
					print("Random Res State: " .. tostring(used_randoms_as_secondaries[random_res]));
				end
			end
		end
	end


	self.realtotalLuxPlacedSoFar = self.totalLuxPlacedSoFar
end



function AssignStartingPlots:PlaceLuxuries_OLD()

	local iW, iH = Map.GetGridSize();
	local lplaced = false;
	local UsedLuxIDs = table.fill(0, self.iNumCivs);
	local SecondaryToAdd = {};

	for i, v in pairs(UsedLuxIDs) do
		print("Used Lux Table: ", i, v);
	end
	

	for loop, reg_data in ipairs(self.regions_sorted_by_type) do
		local region_number = reg_data[1];
		local this_region_luxury = reg_data[2];
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		print("-"); print("Attempting to place Luxury#", this_region_luxury, "at start plot", x, y, "in Region#", region_number);

		local iNumToPlace = 2;
		if self.start_locations == 1 or self.start_locations == 2 then
			iNumToPlace = 3;
		end
		
		rtoplace = 1;

		if self.regionData[region_number][8] < 2.5 then
			print("-"); print("Region#", region_number, "has low rectangle fertility, giving it an extra Luxury at start plot.");
			iNumToPlace = iNumToPlace + 1;
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] + 1;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] + 1;
		end
		if self.regionData[region_number][6] / self.regionTerrainCounts[region_number][2] < 4 then
			print("-"); print("Region#", region_number, "has low per-plot fertility, giving it an extra Luxury at start plot.");
			iNumToPlace = iNumToPlace + 1;
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] + 1;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] + 1;
		end


		local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
		primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(this_region_luxury);
		luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)


		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumToPlace, 1, -1, 0, 0, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
		end

		if iNumLeftToPlace > 0 then

			luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(this_region_luxury, 1, iNumLeftToPlace, 1, -1, 0, 0, shuf_list);
			end
		end

		SecondaryToAdd[region_number] = 1;

		if iNumLeftToPlace > 0 then



			print("#####################"); print("Region#", region_number, "Failed To Place Regional Lux At Start");
			self.luxury_low_fert_compensation[this_region_luxury] = self.luxury_low_fert_compensation[this_region_luxury] - iNumLeftToPlace;
			self.region_low_fert_compensation[region_number] = self.region_low_fert_compensation[region_number] - iNumLeftToPlace;
			print("Left To Place: ", iNumLeftToPlace);
			SecondaryToAdd[region_number] = SecondaryToAdd[region_number] + iNumLeftToPlace;
		end
	end




	if self.resource_setting ~= 1 then
		for region_number = 1, self.iNumCivs do
			local x = self.startingPlots[region_number][1];
			local y = self.startingPlots[region_number][2];
			local use_this_ID;
			local candidate_types, iNumTypesAllowed = {}, 0;
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 2)
			local secondaryPlaced = false;
			local NumSecToAdd = 0;
			local SecondaryCarry = 0;

			NumSecToAdd = SecondaryToAdd[region_number];

			print("-"); print("--- Checking Random Luxes for Second Luxury in Region#", region_number, "Number To Add: ",  NumSecToAdd);


			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true then

					iNumTypesAllowed = iNumTypesAllowed + 1;
					table.insert(candidate_types, res_ID);
				end
			end

			if (self.start_locations ~= 1) and (self.start_locations ~= 2) and (self.start_locations ~= 3) then
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_special_case) do
					if allowed_luxuries[res_ID] == true then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end


			if self._lek_coastal_refish and Map.GetPlot(x, y):IsCoastalLand() then
				iNumTypesAllowed = 0;
				candidate_types = {};
				table.insert(candidate_types, self.whale_ID);
				table.insert(candidate_types, self.pearls_ID);
				table.insert(candidate_types, self.crab_ID);
				table.insert(candidate_types, self.coral_ID);
				iNumTypesAllowed = iNumTypesAllowed + 4;
				print("sapht: forcing a coastal lux (3)")


			end
		

			shuf_luxes = GetShuffledCopyOfTable(candidate_types);

			for loop, use_this_ID in ipairs(shuf_luxes) do

				print("Trying To Place Lux With ID: " .. tostring(use_this_ID));

				SecondaryCarry = 0;

				if NumSecToAdd > 2 then
					SecondaryCarry = NumSecToAdd - 2;
					NumSecToAdd = 2;
				end

				if use_this_ID ~= nil then
					local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
					primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(use_this_ID);
					luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
					local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
					
					NumSecToAdd = iNumLeftToPlace;

					if iNumLeftToPlace > 0 and secondary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						NumSecToAdd = iNumLeftToPlace;
					end
					
					if iNumLeftToPlace > 0 and tertiary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						NumSecToAdd = iNumLeftToPlace;
					end
					
					if iNumLeftToPlace > 0 and quaternary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
					end

					print("iNumLeftToPlace: " .. tostring(iNumLeftToPlace));
					print("SecondaryCarry: " .. tostring(SecondaryCarry));

					NumSecToAdd = iNumLeftToPlace + SecondaryCarry;

					if NumSecToAdd == 0 then
						print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
						secondaryPlaced = true;
						break;
					else
						print("-"); print("Failed To Place All Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number, "Number To Add: ",  NumSecToAdd);
					end
				end
			end

			if (not secondaryPlaced) then
				print("-"); print("--- Checking City State Luxes for Second Luxury in Region#", region_number, "---");
				candidate_types = {};


				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
					if allowed_luxuries[res_ID] == true then
						print("- Found eligible luxury type:", res_ID);
						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			
				shuf_luxes = GetShuffledCopyOfTable(candidate_types);

				for loop, use_this_ID in ipairs(shuf_luxes) do

					print("Trying To Place Lux With ID: " .. tostring(use_this_ID));

					if use_this_ID ~= nil then
						local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
						primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(use_this_ID);
						luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
						local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						if iNumLeftToPlace > 0 and secondary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end
						if iNumLeftToPlace > 0 and tertiary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end
						if iNumLeftToPlace > 0 and quaternary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end

						NumSecToAdd = iNumLeftToPlace;

						if iNumLeftToPlace == 0 then
							print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
							secondaryPlaced = true;
							break;
						end
					end
				end
			end

			if (not secondaryPlaced) then
				print("-"); print("--- Checking Other Players Reigional Luxes for Second Luxury in Region#", region_number, "---");
				candidate_types = {};


				local region_lux_ID = self.region_luxury_assignment[region_number];
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
					if res_ID ~= region_lux_ID then
						if allowed_luxuries[res_ID] == true then
							print("- Found eligible luxury type:", res_ID);
							iNumTypesAllowed = iNumTypesAllowed + 1;
							table.insert(candidate_types, res_ID);
						end
					end
				end

				shuf_luxes = GetShuffledCopyOfTable(candidate_types)

				for loop, use_this_ID in ipairs(shuf_luxes) do

					if use_this_ID ~= nil then

						print("Trying To Place Lux With ID: " .. tostring(use_this_ID));

						local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
						primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(use_this_ID);
						luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 2, false)
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
						local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						if iNumLeftToPlace > 0 and secondary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end
						if iNumLeftToPlace > 0 and tertiary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end
						if iNumLeftToPlace > 0 and quaternary > 0 then
							shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
							iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, NumSecToAdd, 1, -1, 0, 0, shuf_list);
						end

						NumSecToAdd = iNumLeftToPlace;

						if iNumLeftToPlace == 0 then
							print("-"); print("Placed Second Luxury type of ID#", use_this_ID, "for start located at Plot", x, y, " in Region#", region_number);
							break;
						end
					end
				end
			end

			if (not secondaryPlaced) then

				print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible positions!");
			end
		end
	end
	


	for city_state = 1, self.iNumCityStates do

		if self.city_state_validity_table[city_state] == false then

		else

			local region_number = self.city_state_region_assignments[city_state];
			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];
			local start_plot = iW * y + x + 1;
			local coastal_start = false;

			if self.plotDataIsCoastal[start_plot] == true then
				coastal_start = true;

			end
			
			local allowed_luxuries = self:GetListOfAllowableLuxuriesAtCitySite(x, y, 1, coastal_start)
			local lux_possible_for_cs = {};


			local cs_only_types = {};
			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
				if allowed_luxuries[res_ID] == true then
					table.insert(cs_only_types, res_ID);
				end
			end
			local iNumCSAllowed = table.maxn(cs_only_types);
			if iNumCSAllowed > 0 then
				for loop, res_ID in ipairs(cs_only_types) do
					lux_possible_for_cs[res_ID] = 75 / iNumCSAllowed;
				end
			end


			if self.iNumTypesRandom > 0 or region_number > 0 then
				local random_types_allowed = {};
				for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
					if allowed_luxuries[res_ID] == true then
						table.insert(random_types_allowed, res_ID);
					end
				end
				local iNumRandAllowed = table.maxn(random_types_allowed);
				local iNumAllowed = iNumRandAllowed;
				if region_number > 0 then
					iNumAllowed = iNumAllowed + 1;
					local res_ID = self.region_luxury_assignment[region_number];
					if allowed_luxuries[res_ID] == true then
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
				if iNumRandAllowed > 0 then
					for loop, res_ID in ipairs(random_types_allowed) do
						lux_possible_for_cs[res_ID] = 25 / iNumAllowed;
					end
				end
			end


			local iNumAvailableTypes = table.maxn(lux_possible_for_cs);
			if iNumAvailableTypes == 0 then
				print("City State #", city_state, "has poor land, ineligible to receive a Luxury resource.");
			else

				local res_threshold = {};
				local totalWeight, accumulatedWeight = 0, 0;
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					totalWeight = totalWeight + this_weight;
				end
				for res_ID, this_weight in pairs(lux_possible_for_cs) do
					local threshold = (this_weight + accumulatedWeight) * 10000 / totalWeight;
					res_threshold[res_ID] = threshold;
					accumulatedWeight = accumulatedWeight + this_weight;
				end

				local use_this_ID;
				local diceroll = Map.Rand(10000, "Choose resource type - Assign Luxury To City State - Lua");
				for res_ID, threshold in pairs(res_threshold) do
					if diceroll < threshold then
						use_this_ID = res_ID;
						break
					end
				end


				local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
				primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(use_this_ID);
				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 1, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then
					print("Placed Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "located at Plot", x, y);
				else
					print("Place Luxury ID#", use_this_ID, "at City State#", city_state, "in Region#", region_number, "Failed at Plot", x, y);
				end
			end
		end
	end
		

	for region_number, res_ID in ipairs(self.region_luxury_assignment) do
		print("-"); print("- - -"); print("Attempting to place regional luxury #", res_ID, "in Region#", region_number);
		local iNumAlreadyPlaced = self.amounts_of_resources_placed[res_ID + 1];
		local assignment_split = self.luxury_assignment_count[res_ID];
		local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list, iNumLeftToPlace;
		primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(res_ID);
		luxury_plot_lists = self:GenerateLuxuryPlotListsInRegion(region_number)




		local target_list = self:GetRegionLuxuryTargetNumbers()
		local targetNum = math.floor((target_list[self.iNumCivs] + (0.5 * self.luxury_low_fert_compensation[res_ID])) / assignment_split);
		targetNum = targetNum - self.region_low_fert_compensation[region_number];

		targetNum = targetNum - 2;
		if self.resource_setting == 1 or self.resource_setting == 2 then
			targetNum = targetNum - 2;
		elseif self.resource_setting == 3 then
			targetNum = targetNum - 1;
		elseif self.resource_setting == 7 then
			targetNum = targetNum + 1;
		elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
			targetNum = targetNum + 2;
		end

		print("Target Number For Luxuires: ", targetNum);

		local iNumThisLuxToPlace = math.max(2, targetNum);


		

		shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
		iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.4, 2, 0, 3, shuf_list);
		if iNumLeftToPlace > 0 and secondary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.4, 2, 0, 3, shuf_list);
		end
		if iNumLeftToPlace > 0 and tertiary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.5, 2, 0, 2, shuf_list);
		end
		if iNumLeftToPlace > 0 and quaternary > 0 then
			shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.6, 2, 0, 2, shuf_list);
		end

	end

	
	
	

	local iExtraToPlace = 3;
	
	for region_number = 1, self.iNumCivs do
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		local start_plot = iW * y + x + 1;
		local coastal_start = false;
		
		print("++++++++++++++ ADDING REGIONAL RANDOMS ++++++++++++++");
		print("Start Location: ", x, y, "Plot Location: ", start_plot)
		
		if self.plotDataIsCoastal[start_plot] == true then
			coastal_start = true;
			print("Location is coastal");
		else
			print("Location is inland");
		end
		
		local use_this_ID = 0;
		local candidate_types, iNumTypesAllowed = {}, 0;
		local allowed_luxuries = self:GetListOfAllowableLuxuriesNearCitySite(x, y, 6, coastal_start)
		local f, l = 0,0;
		
		print("Used Lux IDs: ", UsedLuxIDs[region_number]);
		
		local iTotalPlaced = 0;
		
		while iTotalPlaced ~= iExtraToPlace do
			candidate_types = {};
			iNumTypesAllowed = 0;

			for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
				if allowed_luxuries[res_ID] == true then

					f = 0;
					f, l = string.find(UsedLuxIDs[region_number], res_ID);
					if f ~= nil then
						print("This region already has lux ID: ", res_ID);
					else

						iNumTypesAllowed = iNumTypesAllowed + 1;
						table.insert(candidate_types, res_ID);
					end
				end
			end
			
			print("Types Allowed: ", iNumTypesAllowed);

			lplaced = false;
			
			while lplaced == false do
				
				if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
				else

					for loop, res_ID in ipairs(self.resourceIDs_assigned_to_cs) do
						if allowed_luxuries[res_ID] == true then

							local f, l = string.find(UsedLuxIDs[region_number], res_ID);
							
							if f ~= 0 then
								print("This region already has lux ID: ", res_ID);
							else

								iNumTypesAllowed = iNumTypesAllowed + 1;
								table.insert(candidate_types, res_ID);
							end
						end
					end
					if iNumTypesAllowed > 0 then
						local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
						use_this_ID = candidate_types[diceroll];
					else

						local region_lux_ID = self.region_luxury_assignment[region_number];
						for loop, res_ID in ipairs(self.resourceIDs_assigned_to_regions) do
							if res_ID ~= region_lux_ID then
								if allowed_luxuries[res_ID] == true then

									local f, l = string.find(UsedLuxIDs[region_number], res_ID);
									
									if f ~= 0 then
										print("This region already has lux ID: ", res_ID);
									else

										iNumTypesAllowed = iNumTypesAllowed + 1;
										table.insert(candidate_types, res_ID);
									end
								end
							end
						end
						if iNumTypesAllowed > 0 then
							local diceroll = 1 + Map.Rand(iNumTypesAllowed, "Choosing second luxury type at a start location - LUA");
							use_this_ID = candidate_types[diceroll];
						else
							print("-"); print("Failed to place a complimetray Luxury type near start in Region#", region_number, "-- no eligible types!"); print("-");
							lplaced = true;
						end
					end
				end

				print("Using Lux with ID: ", use_this_ID);

				if use_this_ID ~= nil then
					local primary, secondary, tertiary, quaternary, luxury_plot_lists, shuf_list;
					primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(use_this_ID);
					luxury_plot_lists = self:GenerateLuxuryPlotListsNearCitySite(x, y, 6, false)
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
					local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, 2, 3, 0, shuf_list);
					if iNumLeftToPlace > 0 and secondary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, 2, 3, 0, shuf_list);
					end
					if iNumLeftToPlace > 0 and tertiary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, 2, 3, 0, shuf_list);
					end
					if iNumLeftToPlace > 0 and quaternary > 0 then
						shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
						iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, 2, 3, 0, shuf_list);
					end
					if iNumLeftToPlace == 0 then
						UsedLuxIDs[region_number] = UsedLuxIDs[region_number] .. ", " .. use_this_ID;
						print("-"); print("Placed Bonus Luxury type of ID#", use_this_ID, "near start located at Plot", x, y, " in Region#", region_number);
						lplaced = true;
					else
						print("-"); print("Failed to place second Luxury type at start in Region#", region_number, "-- no eligible positions!"); print("-");
						lplaced = true;
					end
				end
			end
			iTotalPlaced = iTotalPlaced + 1;
		end

	end
	
	
	
	
	

	if self.iNumTypesRandom > 0 then
		print("* *"); print("* iNumTypesRandom = ", self.iNumTypesRandom); print("* *");





		local world_size_data = self:GetWorldLuxuryTargetNumbers()
		local targetLuxForThisWorldSize = world_size_data[1];
		local loopTarget = world_size_data[2];
		local extraLux = Map.Rand(self.iNumCivs, "Luxury Resource Variance - Place Resources LUA");
		local iNumRandomLuxTarget = targetLuxForThisWorldSize + extraLux - self.totalLuxPlacedSoFar;
		
		print("##########################################");
		print("Traget Lux For This Word Size: ", targetLuxForThisWorldSize);
		print("##########################################");

		if self.iNumTypesRandom * 3 > iNumRandomLuxTarget then
			print ("iNumRandomLuxTarget = " .. tostring(iNumRandomLuxTarget) .. ". Just putting in 3 of each random.");
		end
		
		local iNumRandomLuxPlaced, iNumThisLuxToPlace = 0, 0;

		local random_lux_ratios_table = {
		{1},
		{1, 1},
		{1, 1, 1},
		{1, 1, 1, 1},
		{1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1} };

		for loop, res_ID in ipairs(self.resourceIDs_assigned_to_random) do
			local primary, secondary, tertiary, quaternary, luxury_plot_lists, current_list, iNumLeftToPlace;
			primary, secondary, tertiary, quaternary = self:GetIndicesForLuxuryType(res_ID);
			

			if self.iNumTypesRandom * 3 > iNumRandomLuxTarget then
				iNumThisLuxToPlace = 3;
				print("Argument 1: ",iNumThisLuxToPlace);
			elseif self.iNumTypesRandom > 8 then
				iNumThisLuxToPlace = math.max(4, math.ceil(iNumRandomLuxTarget / 10));
				print("Argument 2: ",iNumThisLuxToPlace);
			else
				local lux_minimum = math.max(3, loopTarget - loop);
				local lux_share_of_remaining = math.ceil(iNumRandomLuxTarget * random_lux_ratios_table[self.iNumTypesRandom][loop]);
				iNumThisLuxToPlace = math.max(lux_minimum, lux_share_of_remaining);
				print("Argument 3: ",iNumThisLuxToPlace);
			end


			local lux_distance = 3;
			
			current_list = self.global_luxury_plot_lists[primary];
			iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumThisLuxToPlace, 0.4, 2, lux_distance, 0, current_list);
			if iNumLeftToPlace > 0 and secondary > 0 then
				current_list = self.global_luxury_plot_lists[secondary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.4, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and tertiary > 0 then
				current_list = self.global_luxury_plot_lists[tertiary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.5, 2, lux_distance, 0, current_list);
			end
			if iNumLeftToPlace > 0 and quaternary > 0 then
				current_list = self.global_luxury_plot_lists[quaternary];
				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(res_ID, 1, iNumLeftToPlace, 0.75, 2, lux_distance, 0, current_list);
			end
			iNumRandomLuxPlaced = iNumRandomLuxPlaced + iNumThisLuxToPlace - iNumLeftToPlace;
			print("-"); print("Random Luxury Target Number:", iNumThisLuxToPlace);
			print("Random Luxury Target Placed:", iNumThisLuxToPlace - iNumLeftToPlace); print("-");
		end

		
		print("-"); print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Target Number:", iNumRandomLuxTarget);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+");
		print("+ Random Luxuries Number Placed:", iNumRandomLuxPlaced);
		print("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"); print("-");
		

	end

	


	if self.iNumTypesSpecialCase > 0 then


	end
end

function AssignStartingPlots:GetListOfAllowableLuxuriesNearCitySite(x, y, radius, loc_is_coastal)

	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	local allowed_luxuries = table.fill(false, 41);

	for ripple_radius = 4, radius do

		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;

					if plotType == PlotTypes.PLOT_OCEAN then
						if terrainType == TerrainTypes.TERRAIN_COAST then
							if plot:IsLake() == false then
								if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE and loc_is_coastal == true then
									allowed_luxuries[self.whale_ID] = true;
									allowed_luxuries[self.pearls_ID] = true;
									allowed_luxuries[self.crab_ID] = true;

								end
							end
						end

					elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then

						allowed_luxuries[self.gold_ID] = true;
						allowed_luxuries[self.silver_ID] = true;
						allowed_luxuries[self.gems_ID] = true;
						if featureType == FeatureTypes.NO_FEATURE then
							allowed_luxuries[self.marble_ID] = true;
							allowed_luxuries[self.copper_ID] = true;
						end
					elseif plotType == PlotTypes.PLOT_LAND then
						if featureType == FeatureTypes.NO_FEATURE then
							if terrainType == TerrainTypes.TERRAIN_TUNDRA then
								allowed_luxuries[self.fur_ID] = true;
								allowed_luxuries[self.silver_ID] = true;
								allowed_luxuries[self.marble_ID] = true;
								allowed_luxuries[self.salt_ID] = true;
								allowed_luxuries[self.copper_ID] = true;

							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								allowed_luxuries[self.gold_ID] = true;
								allowed_luxuries[self.marble_ID] = true;
								allowed_luxuries[self.incense_ID] = true;
								allowed_luxuries[self.salt_ID] = true;
								allowed_luxuries[self.copper_ID] = true;

							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								allowed_luxuries[self.marble_ID] = true;
								allowed_luxuries[self.ivory_ID] = true;
								allowed_luxuries[self.wine_ID] = true;
								allowed_luxuries[self.incense_ID] = true;
								allowed_luxuries[self.salt_ID] = true;

							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								if plot:IsFreshWater() then
									allowed_luxuries[self.sugar_ID] = true;
									allowed_luxuries[self.cotton_ID] = true;
									allowed_luxuries[self.wine_ID] = true;

								else
									allowed_luxuries[self.marble_ID] = true;
									allowed_luxuries[self.ivory_ID] = true;
									allowed_luxuries[self.cotton_ID] = true;
									allowed_luxuries[self.wine_ID] = true;

								end
							end
						elseif featureType == FeatureTypes.FEATURE_MARSH then		
							allowed_luxuries[self.dye_ID] = true;
							allowed_luxuries[self.sugar_ID] = true;
							allowed_luxuries[self.rubber_ID_] = true;
							allowed_luxuries[self.coconut_ID] = true;

						elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
							allowed_luxuries[self.cotton_ID] = true;
							allowed_luxuries[self.incense_ID] = true;
							allowed_luxuries[self.citrus_ID] = true;

						elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
							allowed_luxuries[self.gems_ID] = true;
							allowed_luxuries[self.dye_ID] = true;
							allowed_luxuries[self.spices_ID] = true;
							allowed_luxuries[self.silk_ID] = true;
							allowed_luxuries[self.sugar_ID] = true;
							allowed_luxuries[self.cocoa_ID] = true;
							allowed_luxuries[self.citrus_ID] = true;
							allowed_luxuries[self.truffles_ID] = true;
							allowed_luxuries[self.rubber_ID] = true;
							allowed_luxuries[self.coconut_ID] = true;

						elseif featureType == FeatureTypes.FEATURE_FOREST then		
							allowed_luxuries[self.fur_ID] = true;
							allowed_luxuries[self.dye_ID] = true;
							allowed_luxuries[self.rubber_ID] = true;
							allowed_luxuries[self.coconut_ID] = true;

							if terrainType == TerrainTypes.TERRAIN_TUNDRA then
								allowed_luxuries[self.silver_ID] = true;

							else
								allowed_luxuries[self.spices_ID] = true;
								allowed_luxuries[self.silk_ID] = true;
								allowed_luxuries[self.citrus_ID] = true;
								allowed_luxuries[self.truffles_ID] = true;
								allowed_luxuries[self.rubber_ID] = true;
								allowed_luxuries[self.coconut_ID] = true;

							end
						end
					end
					currentX, currentY = nextX, nextY;
				end
			end
		end
	end




	return allowed_luxuries
end

function AssignStartingPlots:GenerateLuxuryPlotListsNearCitySite(x, y, radius, bRemoveFeatureIce)




	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;

	local region_coast, region_marsh, region_flood_plains, region_tundra_flat_including_forests = {}, {}, {}, {};
	local region_hills_open, region_hills_covered, region_hills_jungle, region_hills_forest = {}, {}, {}, {};
	local region_desert_flat_no_feature, region_plains_flat_no_feature, region_jungle_flat = {}, {}, {};
	local region_forest_flat, region_forest_flat_but_not_tundra = {}, {};
	local region_dry_grass_flat_no_feature, region_fresh_water_grass_flat_no_feature = {}, {};



	
	for ripple_radius = 4, radius do
		local ripple_value = radius - ripple_radius + 1;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
				if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					
					local plotType = plot:GetPlotType()
					local terrainType = plot:GetTerrainType()
					local featureType = plot:GetFeatureType()
					local plotIndex = realY * iW + realX + 1;
					

					if bRemoveFeatureIce == true then
						if featureType == FeatureTypes.FEATURE_ICE then
							plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1);
						end
						

					else
						if plotType == PlotTypes.PLOT_OCEAN then
							if terrainType == TerrainTypes.TERRAIN_COAST then
								if plot:IsLake() == false then
									if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
										table.insert(region_coast, plotIndex);
									end
								end
							end
						elseif plotType == PlotTypes.PLOT_HILLS and terrainType ~= TerrainTypes.TERRAIN_SNOW then
							if featureType == FeatureTypes.NO_FEATURE then
								table.insert(region_hills_open, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
								table.insert(region_hills_jungle, plotIndex);
								table.insert(region_hills_covered, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FOREST then		
								table.insert(region_hills_forest, plotIndex);
								table.insert(region_hills_covered, plotIndex);
							end
						elseif plotType == PlotTypes.PLOT_LAND then
							if featureType == FeatureTypes.NO_FEATURE then
								if terrainType == TerrainTypes.TERRAIN_TUNDRA then
									table.insert(region_tundra_flat_including_forests, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_DESERT then
									table.insert(region_desert_flat_no_feature, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
									table.insert(region_plains_flat_no_feature, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_GRASS then
									if plot:IsFreshWater() then
										table.insert(region_fresh_water_grass_flat_no_feature, plotIndex);
									else
										table.insert(region_dry_grass_flat_no_feature, plotIndex);
									end
								end
							elseif featureType == FeatureTypes.FEATURE_MARSH then		
								table.insert(region_marsh, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then		
								table.insert(region_flood_plains, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_JUNGLE then		
								table.insert(region_jungle_flat, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FOREST then		
								table.insert(region_forest_flat, plotIndex);
								if terrainType == TerrainTypes.TERRAIN_TUNDRA then
									table.insert(region_tundra_flat_including_forests, plotIndex);
								else
									table.insert(region_forest_flat_but_not_tundra, plotIndex);
								end
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
	end
			
	local results_table = {
	region_coast,
	region_marsh,
	region_flood_plains,
	region_hills_open,
	region_hills_covered,
	region_hills_jungle,
	region_hills_forest,
	region_jungle_flat,
	region_forest_flat,
	region_desert_flat_no_feature,
	region_plains_flat_no_feature,
	region_dry_grass_flat_no_feature,
	region_fresh_water_grass_flat_no_feature,
	region_tundra_flat_including_forests,
	region_forest_flat_but_not_tundra
	};
	return results_table
end

function AssignStartingPlots:PlaceMarble()
	local marble_already_placed = self.amounts_of_resources_placed[self.marble_ID + 1];
	local marble_target = math.ceil(self.iNumCivs * 0.85);
	if self.resource_setting == 1 then
		marble_target = math.ceil(self.iNumCivs * 0.5);
	elseif self.resource_setting == 3 then
		marble_target = math.ceil(self.iNumCivs * 0.9);
	end	
	local iNumMarbleToPlace = math.max(2, marble_target - marble_already_placed);
	local iW, iH = Map.GetGridSize();
	local iNumLeftToPlace = iNumMarbleToPlace;
	local iNumPlots = table.maxn(self.marble_list);
	if iNumPlots < 1 then

		return
	end

	for place_resource = 1, iNumMarbleToPlace do
		for loop, plotIndex in ipairs(self.marble_list) do
			if self.marbleData[plotIndex] == 0 and self.distanceData[plotIndex] < 90 then			
				local x = (plotIndex - 1) % iW;
				local y = (plotIndex - x - 1) / iW;
				local res_plot = Map.GetPlot(x, y)
				if res_plot:GetResourceType(-1) == -1 then
					res_plot:SetResourceType(self.marble_ID, 1);
					self.amounts_of_resources_placed[self.marble_ID + 1] = self.amounts_of_resources_placed[self.marble_ID + 1] + 1;

					self.totalLuxPlacedSoFar = self.totalLuxPlacedSoFar + 1;
					iNumLeftToPlace = iNumLeftToPlace - 1;

					self:PlaceResourceImpact(x, y, 2, 1)
					self:PlaceResourceImpact(x, y, 7, 6)
					break
				end
			end
		end
	end
	if iNumLeftToPlace > 0 then
		print("Failed to place", iNumLeftToPlace, "units of Marble.");
	end
end

function AssignStartingPlots:PlaceSmallQuantitiesOfStrategics(frequency, plot_list)

	if plot_list == nil then

		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumToPlace = math.ceil(iNumTotalPlots / frequency);

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues()
	

	local current_index = 1;
	for place_resource = 1, iNumToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				if self.strategicData[plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)
					if res_plot:GetResourceType(-1) == -1 then

						local selected_ID = -1;
						local selected_quantity = 2;
						local plotType = res_plot:GetPlotType()
						local terrainType = res_plot:GetTerrainType()
						local featureType = res_plot:GetFeatureType()
						if featureType == FeatureTypes.FEATURE_MARSH then
							local diceroll = Map.Rand(4, "Resource selection - Place Small Quantities LUA");
							if diceroll == 0 then
								selected_ID = self.iron_ID;
								selected_quantity = iron_amt;
							elseif diceroll == 1 then
								selected_ID = self.coal_ID;
								selected_quantity = coal_amt;
							else
								selected_ID = self.oil_ID;
								selected_quantity = oil_amt;
							end
						elseif featureType == FeatureTypes.FEATURE_JUNGLE then
							local diceroll = Map.Rand(4, "Resource selection - Place Small Quantities LUA");
							if diceroll == 0 then
								if plotType == PlotTypes.PLOT_HILLS then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							elseif diceroll == 1 then
								selected_ID = self.coal_ID;
								selected_quantity = coal_amt;
							else
								selected_ID = self.aluminum_ID;
								selected_quantity = alum_amt;
							end
						elseif featureType == FeatureTypes.FEATURE_FOREST then
							local diceroll = Map.Rand(4, "Resource selection - Place Small Quantities LUA");
							if diceroll == 0 then
								selected_ID = self.uranium_ID;
								selected_quantity = uran_amt;
							elseif diceroll == 1 then
								selected_ID = self.coal_ID;
								selected_quantity = coal_amt;
							else
								selected_ID = self.iron_ID;
								selected_quantity = iron_amt;
							end
						elseif featureType == FeatureTypes.NO_FEATURE then
							if plotType == PlotTypes.PLOT_HILLS then
								if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
									local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
									if diceroll < 2 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									elseif diceroll == 2 then
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;										
									else
										selected_ID = self.horse_ID;
										selected_quantity = horse_amt;
									end
								else
									local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
									if diceroll < 2 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.coal_ID;
										selected_quantity = coal_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_GRASS then
								if res_plot:IsFreshWater() then
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								else
									local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
									if diceroll < 2 then
										selected_ID = self.iron_ID;
										selected_quantity = iron_amt;
									else
										selected_ID = self.horse_ID;
										selected_quantity = horse_amt;
									end
								end
							elseif terrainType == TerrainTypes.TERRAIN_PLAINS then
								local diceroll = Map.Rand(5, "Resource selection - Place Small Quantities LUA");
								if diceroll < 2 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								else
									selected_ID = self.horse_ID;
									selected_quantity = horse_amt;
								end
							elseif terrainType == TerrainTypes.TERRAIN_DESERT then
								local diceroll = Map.Rand(3, "Resource selection - Place Small Quantities LUA");
								if diceroll == 0 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll == 1 then
									selected_ID = self.aluminum_ID;
									selected_quantity = alum_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							else
								local diceroll = Map.Rand(4, "Resource selection - Place Small Quantities LUA");
								if diceroll == 0 then
									selected_ID = self.iron_ID;
									selected_quantity = iron_amt;
								elseif diceroll == 1 then
									selected_ID = self.uranium_ID;
									selected_quantity = uran_amt;
								else
									selected_ID = self.oil_ID;
									selected_quantity = oil_amt;
								end
							end
						end

						if selected_ID ~= -1 then	
							local strat_radius = Map.Rand(4, "Resource Radius - Place Small Quantities LUA");
							if strat_radius > 2 then
								strat_radius = 1;
							end
							res_plot:SetResourceType(selected_ID, selected_quantity);
							self:PlaceResourceImpact(x, y, 1, strat_radius);
							placed_this_res = true;
							self.amounts_of_resources_placed[selected_ID + 1] = self.amounts_of_resources_placed[selected_ID + 1] + selected_quantity;
						end
					end
				end
			end
		end
	end
end



function AssignStartingPlots:PlaceFishMainland(frequency, plot_list)

	if plot_list == nil then

		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumFishToPlace = math.ceil(iNumTotalPlots / frequency);
	

	local current_index = 1;
	for place_resource = 1, iNumFishToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];

				if self.fishData[plotIndex] == 0 then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)

					local featureType = res_plot:GetFeatureType()

					if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then

						if res_plot:GetResourceType(-1) == -1 then

							local fish_radius = Map.Rand(0, "Fish Radius - Place Fish LUA") + 1;




							res_plot:SetResourceType(self.fish_ID, 1);
							self:PlaceResourceImpact(x, y, 4, fish_radius);
							placed_this_res = true;
							self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
						end
					end
				end
			end
		end
	end
end



function AssignStartingPlots:PlaceFish(frequency, plot_list)

	if plot_list == nil then

		return
	end
	local iW, iH = Map.GetGridSize();
	local iNumTotalPlots = table.maxn(plot_list);
	local iNumFishToPlace = math.ceil(iNumTotalPlots / frequency);
	local bMainlandCoast = false;


	local current_index = 1;
	for place_resource = 1, iNumFishToPlace do
		local placed_this_res = false;
		if current_index <= iNumTotalPlots then
			for index_to_check = current_index, iNumTotalPlots do
				if placed_this_res == true then
					break
				else
					current_index = current_index + 1;
				end
				local plotIndex = plot_list[index_to_check];
				bMainlandCoast = false;

				if self.method == 1 then
					bMainlandCoast = self.mainland_coast_list[plotIndex];
				end

				if self.fishData[plotIndex] == 0 and bMainlandCoast == false then
					local x = (plotIndex - 1) % iW;
					local y = (plotIndex - x - 1) / iW;
					local res_plot = Map.GetPlot(x, y)
					if res_plot:GetResourceType(-1) == -1 then

						local fish_radius = Map.Rand(4, "Fish Radius - Place Fish LUA") + 1;




						res_plot:SetResourceType(self.fish_ID, 1);
						self:PlaceResourceImpact(x, y, 4, fish_radius);
						placed_this_res = true;
						self.amounts_of_resources_placed[self.fish_ID + 1] = self.amounts_of_resources_placed[self.fish_ID + 1] + 1;
					end
				end
			end
		end
	end
end

function AssignStartingPlots:PlaceSexyBonusAtCivStarts()




	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();
	local odd = self.firstRingYIsOdd;
	local even = self.firstRingYIsEven;
	local nextX, nextY, plot_adjustments;
	
	local bonus_type_associated_with_region_type = {self.deer_ID, self.banana_ID, 
	self.deer_ID, self.wheat_ID, self.maize_ID, self.sheep_ID, self.wheat_ID, self.cow_ID, self.cow_ID, self.wheat_ID, self.hardwood_ID, self.maize_ID,};
	
	for region_number = 1, self.iNumCivs do
		local x = self.startingPlots[region_number][1];
		local y = self.startingPlots[region_number][2];
		local region_type = self.regionTypes[region_number];
		local use_this_ID = bonus_type_associated_with_region_type[region_type];
		local plot_list, fish_list = {}, {};

		local ripple_radius = 2;
		local currentX = x - ripple_radius;
		local currentY = y;
		for direction_index = 1, 6 do
			for plot_to_handle = 1, ripple_radius do
			 	if currentY / 2 > math.floor(currentY / 2) then
					plot_adjustments = odd[direction_index];
				else
					plot_adjustments = even[direction_index];
				end
				nextX = currentX + plot_adjustments[1];
				nextY = currentY + plot_adjustments[2];
				if wrapX == false and (nextX < 0 or nextX >= iW) then

				elseif wrapY == false and (nextY < 0 or nextY >= iH) then

				else
					local realX = nextX;
					local realY = nextY;
					if wrapX then
						realX = realX % iW;
					end
					if wrapY then
						realY = realY % iH;
					end

					local plot = Map.GetPlot(realX, realY);
					local featureType = plot:GetFeatureType()
					if plot:GetResourceType(-1) == -1 and featureType ~= FeatureTypes.FEATURE_OASIS then
						local plotType = plot:GetPlotType()
						local terrainType = plot:GetTerrainType()
						local plotIndex = realY * iW + realX + 1;

						if use_this_ID == self.deer_ID then
							if featureType == FeatureTypes.FEATURE_FOREST then
								table.insert(plot_list, plotIndex);
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND then
								table.insert(plot_list, plotIndex);
							end
						elseif use_this_ID == self.hardwood_ID then
							if featureType == FeatureTypes.FEATURE_FOREST then
								table.insert(plot_list, plotIndex);
							elseif terrainType == TerrainTypes.TERRAIN_TUNDRA and plotType == PlotTypes.PLOT_LAND then
								table.insert(plot_list, plotIndex);
							end
						elseif use_this_ID == self.banana_ID then
							if featureType == FeatureTypes.FEATURE_JUNGLE then
								table.insert(plot_list, plotIndex);
							end
						elseif use_this_ID == self.wheat_ID then
							if plotType == PlotTypes.PLOT_LAND then
								if terrainType == TerrainTypes.TERRAIN_PLAINS and featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
									table.insert(plot_list, plotIndex);
								elseif terrainType == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.maize_ID then
							if plotType == PlotTypes.PLOT_LAND then
								if terrainType == TerrainTypes.TERRAIN_PLAINS and featureType == FeatureTypes.NO_FEATURE then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.sheep_ID then
							if plotType == PlotTypes.PLOT_HILLS and featureType == FeatureTypes.NO_FEATURE then
								if terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_TUNDRA then
									table.insert(plot_list, plotIndex);
								end
							end
						elseif use_this_ID == self.cow_ID then
							if plotType == PlotTypes.PLOT_LAND then
								if terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS then
									if featureType == FeatureTypes.NO_FEATURE then
										table.insert(plot_list, plotIndex);
									end
								end
							end
						end
						if plotType == PlotTypes.PLOT_OCEAN then
							if not plot:IsLake() then
								if featureType ~= self.feature_atoll and featureType ~= FeatureTypes.FEATURE_ICE then
									if terrainType == TerrainTypes.TERRAIN_COAST then
										table.insert(fish_list, plotIndex);
									end
								end
							end
						end
					end
				end
				currentX, currentY = nextX, nextY;
			end
		end
		local iNumCandidates = table.maxn(plot_list);
		if iNumCandidates > 0 then

			local shuf_list = GetShuffledCopyOfTable(plot_list)
			local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
			if iNumCandidates > 1 and use_this_ID == self.sheep_ID then


				iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, 1, 1, 1, -1, 0, 0, shuf_list);
			end
		else
			local iFishCandidates = table.maxn(fish_list);
			if iFishCandidates > 0 then

				local shuf_list = GetShuffledCopyOfTable(fish_list)
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.fish_ID, 1, 1, 1, -1, 0, 0, shuf_list);
			end
		end
	end
end

function AssignStartingPlots:AddExtraBonusesToHillsRegions()


	local iW, iH = Map.GetGridSize();
	local wrapX = Map:IsWrapX();
	local wrapY = Map:IsWrapY();

	local hills_regions, iNumHillsRegions = {}, 0;
	for region_number = 1, self.iNumCivs do
		if self.regionTypes[region_number] == 5 then
			iNumHillsRegions = iNumHillsRegions + 1;
			table.insert(hills_regions, region_number);
		end
	end
	if iNumHillsRegions == 0 then
		return
	end

	local shuffled_hills_regions = GetShuffledCopyOfTable(hills_regions)
	for loop, region_number in ipairs(shuffled_hills_regions) do
		local iWestX = self.regionData[region_number][1];
		local iSouthY = self.regionData[region_number][2];
		local iWidth = self.regionData[region_number][3];
		local iHeight = self.regionData[region_number][4];
		local iAreaID = self.regionData[region_number][5];

		local terrainCounts = self.regionTerrainCounts[region_number];

		local areaPlots = terrainCounts[2];

		local flatlandsCount = terrainCounts[4];
		local hillsCount = terrainCounts[5];
		local peaksCount = terrainCounts[6];




		local grassCount = terrainCounts[11];
		local plainsCount = terrainCounts[12];













		local hills_ratio = (hillsCount + peaksCount) / areaPlots;
		local farm_ratio = (grassCount + plainsCount) / areaPlots;
		if self.method == 3 then
			hills_ratio = (hillsCount + peaksCount) / (hillsCount + peaksCount + flatlandsCount);
			farm_ratio = (grassCount + plainsCount) / (hillsCount + peaksCount + flatlandsCount);
		end


		local infertility_quotient = 1 + math.max(0, hills_ratio - farm_ratio);
		

		


		local dry_hills, flat_plains, flat_grass, flat_tundra, jungles, forests = {}, {}, {}, {}, {}, {};
		for region_loop_y = 0, iHeight - 1 do
			for region_loop_x = 0, iWidth - 1 do
				local x = (region_loop_x + iWestX) % iW;
				local y = (region_loop_y + iSouthY) % iH;
				local plot = Map.GetPlot(x, y);
				local plotIndex = y * iW + x + 1;
				local area_of_plot = plot:GetArea();
				local plotType = plot:GetPlotType()
				local terrainType = plot:GetTerrainType()
				local featureType = plot:GetFeatureType()
				if plotType == PlotTypes.PLOT_LAND or plotType == PlotTypes.PLOT_HILLS then

					if (area_of_plot == iAreaID) or (iAreaID == -1) then
						if plot:GetResourceType(-1) == -1 then
							if featureType == FeatureTypes.FEATURE_JUNGLE then
								table.insert(jungles, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FOREST then
								table.insert(forests, plotIndex);
							elseif featureType == FeatureTypes.FEATURE_FLOOD_PLAINS then
								table.insert(flat_plains, plotIndex);
							elseif featureType == FeatureTypes.NO_FEATURE then
								if plotType == PlotTypes.PLOT_HILLS then
									if (terrainType == TerrainTypes.TERRAIN_GRASS or terrainType == TerrainTypes.TERRAIN_PLAINS or terrainType == TerrainTypes.TERRAIN_TUNDRA) then
										if plot:IsFreshWater() == false then
											table.insert(dry_hills, plotIndex);
										end
									end
								elseif plotType == PlotTypes.PLOT_LAND then
									if terrainType == TerrainTypes.TERRAIN_PLAINS then
										table.insert(flat_plains, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_DESERT and plot:IsFreshWater() then
										table.insert(flat_plains, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_GRASS then
										table.insert(flat_grass, plotIndex);
									elseif terrainType == TerrainTypes.TERRAIN_TUNDRA then
										table.insert(flat_tundra, plotIndex);
									end
								end
							end
						end
					end
				end
			end
		end
		









		

		if table.maxn(dry_hills) > 0 then
			local resources_to_place = {
			{self.sheep_ID, 1, 100, 1, 1} };
			self:ProcessResourceList(9 / infertility_quotient, 3, dry_hills, resources_to_place)
		end
		if table.maxn(jungles) > 0 then
			local resources_to_place = {
			{self.banana_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(14 / infertility_quotient, 3, jungles, resources_to_place)
		end
		if table.maxn(flat_tundra) > 0 then
			local resources_to_place = {
			{self.deer_ID, 1, 100, 0, 1} };
			self:ProcessResourceList(14 / infertility_quotient, 3, flat_tundra, resources_to_place)
		end
		if table.maxn(flat_tundra) > 0 then
			local resources_to_place = {
			{self.hardwood_ID, 1, 100, 0, 1} };
			self:ProcessResourceList(14 / infertility_quotient, 3, flat_tundra, resources_to_place)
		end
		if table.maxn(flat_plains) > 0 then
			local resources_to_place = {
			{self.wheat_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(18 / infertility_quotient, 3, flat_plains, resources_to_place)
		end
		if table.maxn(flat_plains) > 0 then
			local resources_to_place = {
			{self.maize_ID, 1, 100, 0, 2} };
			self:ProcessResourceList(18 / infertility_quotient, 3, flat_plains, resources_to_place)
		end
		if table.maxn(flat_grass) > 0 then
			local resources_to_place = {
			{self.cow_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(20 / infertility_quotient, 3, flat_grass, resources_to_place)
		end
		if table.maxn(forests) > 0 then
			local resources_to_place = {
			{self.deer_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(24 / infertility_quotient, 3, forests, resources_to_place)
		end
		if table.maxn(forests) > 0 then
			local resources_to_place = {
			{self.hardwood_ID, 1, 100, 1, 2} };
			self:ProcessResourceList(24 / infertility_quotient, 3, forests, resources_to_place)
		end
		



	end
end

function AssignStartingPlots:AddModernMinorStrategicsToCityStates()

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetSmallStrategicResourceQuantityValues()
	for city_state = 1, self.iNumCityStates do

		if self.city_state_validity_table[city_state] == false then

		else

			local x = self.cityStatePlots[city_state][1];
			local y = self.cityStatePlots[city_state][2];

			local diceroll = Map.Rand(4, "Choose resource type - CS Strategic LUA");
			if diceroll > 0 then

				local use_this_ID, res_amt, luxury_plot_lists, shuf_list;
				local primary, secondary, tertiary, quaternary, quinternary, sexternary = 0, 0, 0, 0, 0, 0;
				if diceroll == 1 then
					use_this_ID = self.coal_ID;
					res_amt = coal_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 4, 5, 14, 12, 11, 10;
				elseif diceroll == 2 then
					use_this_ID = self.oil_ID;
					res_amt = oil_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 10, 2, 14, 15, 12, 11;
				elseif diceroll == 3 then
					use_this_ID = self.aluminum_ID;
					res_amt = alum_amt;
					primary, secondary, tertiary, quaternary, quinternary, sexternary = 4, 5, 14, 10, 11, 12;
				end


				luxury_plot_lists = self:GenerateLuxuryPlotListsAtCitySite(x, y, 3, false)
				shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[primary])
				local iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				if iNumLeftToPlace > 0 and secondary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[secondary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and tertiary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[tertiary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quaternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quaternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and quinternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[quinternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace > 0 and sexternary > 0 then
					shuf_list = GetShuffledCopyOfTable(luxury_plot_lists[sexternary])
					iNumLeftToPlace = self:PlaceSpecificNumberOfResources(use_this_ID, res_amt, 1, 1, -1, 0, 0, shuf_list);
				end
				if iNumLeftToPlace == 0 then

				end
			else

			end
		end
	end
end

function AssignStartingPlots:PlaceOilInTheSea()





	local sea_oil_amt = 4;
	if self.resource_setting == 1 or self.resource_setting == 2 then
		sea_oil_amt = sea_oil_amt - 2;
	elseif self.resource_setting == 3 then
		sea_oil_amt = sea_oil_amt - 1;
	elseif self.resource_setting == 7 then
		sea_oil_amt = sea_oil_amt + 1;
	elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
		sea_oil_amt = sea_oil_amt + 2;
	end
	local iNumLandOilUnits = self.amounts_of_resources_placed[self.oil_ID + 1];
	local iNumToPlace = math.floor((iNumLandOilUnits / 2) / (sea_oil_amt / 2));

	print("+++++++++++++++++++++++++++++++++++++++++++++ Adding Oil resources to the Sea +++++++++++++++++++++++++++++++++++++++++++++");
	print("Land Oil Count: " .. tostring(iNumLandOilUnits));
	print("Number to Place: " .. tostring(iNumToPlace));
	iNumLeftToPlace = self:PlaceSpecificNumberOfResources(self.oil_ID, sea_oil_amt, iNumToPlace, 1, 8, 7, 10, self.coast_list);
	print("Number not Placed: " .. tostring(iNumLeftToPlace));
end

function AssignStartingPlots:FixResourceGraphics()


	


	

		 
	local iW, iH = Map.GetGridSize()
	for y = 0, iH - 1 do
		for x = 0, iW - 1 do
			
			local plot = Map.GetPlot(x, y)
			local res_ID = plot:GetResourceType(-1)
			local featureType = plot:GetFeatureType()
			

			if res_ID == self.marble_ID or 
			   res_ID == self.gold_ID or 
			   res_ID == self.silver_ID or 
			   res_ID == self.copper_ID or 
			   res_ID == self.gems_ID or 
			   res_ID == self.salt_ID or 
			   res_ID == self.lapis_ID or 
			   res_ID == self.jade_ID or 
			   res_ID == self.obsidian_ID or
			   res_ID == self.amber_ID then 
			   

				if (featureType ~= FeatureTypes.FEATURE_FOREST) and (featureType ~= FeatureTypes.FEATURE_JUNGLE) then
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end


				

			elseif res_ID == self.cocoa_ID or 
			       res_ID == self.citrus_ID or 
				   res_ID == self.spices_ID or 
				   res_ID == self.sugar_ID or 
				   res_ID == self.truffles_ID or 
				   res_ID == self.silk_ID or 
				   res_ID == self.dye_ID or 
				   res_ID == self.fur_ID or
				   red_ID == self.coconut_ID or
				   red_ID == self.rubber_ID or
				   res_ID == self.hardwood_ID or
				   res_ID == self.deer_ID then
				
				if (featureType ~= FeatureTypes.FEATURE_FOREST) then
					plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
				elseif (featureType ~= FeatureTypes.FEATURE_JUNGLE) then
					plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
				end
				
				if res_ID == self.fur_ID then

					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)
				end
				

				local lat = 0
				if (y >= (iH/2)) then
					lat = math.abs((iH/2) - y)/(iH/2)
				else
					lat = math.abs((iH/2) - (y + 1))/(iH/2)
				end
				local AvgJungleRange = 0




				local rain = Map.GetCustomOption(2)	
				if rain == 1 then		

					AvgJungleRange = 0.08
				elseif rain == 3 then	

					AvgJungleRange = 0.25
				else					

					AvgJungleRange = 0.12
				end
				

				if (featureType == FeatureTypes.FEATURE_MARSH) then
					if res_ID == self.sugar_ID or res_ID == self.spices_ID or res_ID == self.dye_ID or res_ID == self.rubber_ID or res_ID == self.coconut_ID then

					else

						if lat <= AvgJungleRange then
							if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
								plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
								plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
							else
								plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
							end
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end	
					end
				else

					if lat <= AvgJungleRange then
						if res_ID ~= self.deer_ID and res_ID ~= self.fur_ID then
							plot:SetFeatureType(FeatureTypes.FEATURE_JUNGLE, -1)
							plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
						else
							plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
						end
					else
						plot:SetFeatureType(FeatureTypes.FEATURE_FOREST, -1)
					end		
				end
				

			elseif res_ID == self.incense_ID or 
			       res_ID == self.ivory_ID or 
				   res_ID == self.wine_ID or 
				   res_ID == self.olives_ID or 
				   res_ID == self.coffee_ID or
				   res_ID == self.tobacco_ID or 
				   res_ID == self.tea_ID or 
				   res_ID == self.perfume_ID or 
				   res_ID == self.cotton_ID then 
				


					plot:SetPlotType(PlotTypes.PLOT_LAND, false, true)

				

				if (featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS) then	
					plot:SetFeatureType(FeatureTypes.NO_FEATURE, -1)
				end
				
				if res_ID == self.incense_ID then



					if terrainType == TerrainTypes.TERRAIN_GRASS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
					end
					if terrainType == TerrainTypes.TERRAIN_DESERT and featureType ~= FeatureTypes.FEATURE_FLOOD_PLAINS then
						plot:SetTerrainType(TerrainTypes.TERRAIN_PLAINS, false, true)
					end
				end
			end
		end
	end























end

function AssignStartingPlots:PrintFinalResourceTotalsToLog()
	print("-");
	print("--- Table of Results, New Start Finder ---");
	for loop, startData in ipairs(self.startingPlots) do
		print("-");
		print("Region#", loop, " has start plot at: ", startData[1], startData[2], "with Fertility Rating of ", startData[3]);
	end
	print("-");
	print("--- End of Start Finder Results Table ---");
	print("-");
	print("-");
	print("--- Table of Final Results, City State Placements ---");
	print("-");
	for cs_number = 1, self.iNumCityStates do
		if self.city_state_validity_table[cs_number] == true then
			local data_table = self.cityStatePlots[cs_number];
			local x = data_table[1];
			local y = data_table[2];
			local regNum = data_table[3];
			print("- City State", cs_number, "in Region", regNum, "is located at Plot", x, y);
		else
			print("- City State", cs_number, "was discarded due to overcrowding.");
		end
	end
	print("-");
	print("- - - - -");
	print("-");
	print("--- Table of Final Results, Resource Distribution ---");
	print("-");
	print("- LUXURY Resources -");

	print(self.whale_ID,    "Whale...: ", self.amounts_of_resources_placed[self.whale_ID + 1])
	print(self.pearls_ID,   "Pearls..: ", self.amounts_of_resources_placed[self.pearls_ID + 1])
	print(self.gold_ID,     "Gold....: ", self.amounts_of_resources_placed[self.gold_ID + 1])
	print(self.silver_ID,   "Silver..: ", self.amounts_of_resources_placed[self.silver_ID + 1])
	print(self.gems_ID,     "Gems....: ", self.amounts_of_resources_placed[self.gems_ID + 1])
	print(self.marble_ID,   "Marble..: ", self.amounts_of_resources_placed[self.marble_ID + 1])
	print(self.ivory_ID,    "Ivory...: ", self.amounts_of_resources_placed[self.ivory_ID + 1])
	print(self.fur_ID,      "Fur.....: ", self.amounts_of_resources_placed[self.fur_ID + 1])
	print(self.dye_ID,      "Dye.....: ", self.amounts_of_resources_placed[self.dye_ID + 1])
	print(self.spices_ID,   "Spices..: ", self.amounts_of_resources_placed[self.spices_ID + 1])
	print(self.silk_ID,     "Silk....: ", self.amounts_of_resources_placed[self.silk_ID + 1])
	print(self.sugar_ID,    "Sugar...: ", self.amounts_of_resources_placed[self.sugar_ID + 1])
	print(self.cotton_ID,   "Cotton..: ", self.amounts_of_resources_placed[self.cotton_ID + 1])
	print(self.wine_ID,     "Wine....: ", self.amounts_of_resources_placed[self.wine_ID + 1])
	print(self.incense_ID,  "Incense.: ", self.amounts_of_resources_placed[self.incense_ID + 1])
	print("- Expansion LUXURY Resources -");
	print(self.copper_ID,   "Copper..: ", self.amounts_of_resources_placed[self.copper_ID + 1])
	print(self.salt_ID,     "Salt....: ", self.amounts_of_resources_placed[self.salt_ID + 1])
	print(self.crab_ID,     "Crab....: ", self.amounts_of_resources_placed[self.crab_ID + 1])
	print(self.truffles_ID, "Truffles: ", self.amounts_of_resources_placed[self.truffles_ID + 1])
	print(self.citrus_ID,   "Citrus..: ", self.amounts_of_resources_placed[self.citrus_ID + 1])
	print(self.cocoa_ID,    "Cocoa...: ", self.amounts_of_resources_placed[self.cocoa_ID + 1])


	if self.bModLuxes == true then
		print("- Mod LUXURY Resources -")
		print(self.coffee_ID,   "Coffee..: ", self.amounts_of_resources_placed[self.coffee_ID + 1])
		print(self.tea_ID,      "Tea.....: ", self.amounts_of_resources_placed[self.tea_ID + 1])
		print(self.tobacco_ID,  "Tobacco.: ", self.amounts_of_resources_placed[self.tobacco_ID + 1])
		print(self.amber_ID,    "Amber...: ", self.amounts_of_resources_placed[self.amber_ID + 1])
		print(self.jade_ID,     "Jade....: ", self.amounts_of_resources_placed[self.jade_ID + 1])
		print(self.olives_ID,   "Olives..: ", self.amounts_of_resources_placed[self.olives_ID + 1])
		print(self.perfume_ID,  "Perfume.: ", self.amounts_of_resources_placed[self.perfume_ID + 1])
		print(self.coral_ID,  	"Coral...: ", self.amounts_of_resources_placed[self.coral_ID + 1])
		print(self.lapis_ID,  	"Lapis...: ", self.amounts_of_resources_placed[self.lapis_ID + 1])
		print(self.obsidian_ID,  "Obsidian: ", self.amounts_of_resources_placed[self.obsidian_ID + 1])
		print(self.rubber_ID,    "Rubber...: ", self.amounts_of_resources_placed[self.rubber_ID + 1])
		print(self.coconut_ID,    "Rubber...: ", self.amounts_of_resources_placed[self.coconut_ID + 1])
	end

	print("-")

	print("+ TOTAL.Lux: ", self.realtotalLuxPlacedSoFar)

	print("-");
	print("- STRATEGIC Resources -");
	print(self.iron_ID,     "Iron....: ", self.amounts_of_resources_placed[self.iron_ID + 1])
	print(self.horse_ID,    "Horse...: ", self.amounts_of_resources_placed[self.horse_ID + 1])
	print(self.coal_ID,     "Coal....: ", self.amounts_of_resources_placed[self.coal_ID + 1])
	print(self.oil_ID,      "Oil.....: ", self.amounts_of_resources_placed[self.oil_ID + 1])
	print(self.aluminum_ID, "Aluminum: ", self.amounts_of_resources_placed[self.aluminum_ID + 1])
	print(self.uranium_ID,  "Uranium.: ", self.amounts_of_resources_placed[self.uranium_ID + 1])
	print("-");
	print("- BONUS Resources -");
	print(self.wheat_ID,    "Wheat...: ", self.amounts_of_resources_placed[self.wheat_ID + 1])
	print(self.cow_ID,      "Cow.....: ", self.amounts_of_resources_placed[self.cow_ID + 1])
	print(self.sheep_ID,    "Sheep...: ", self.amounts_of_resources_placed[self.sheep_ID + 1])
	print(self.deer_ID,     "Deer....: ", self.amounts_of_resources_placed[self.deer_ID + 1])
	print(self.banana_ID,   "Banana..: ", self.amounts_of_resources_placed[self.banana_ID + 1])
	print(self.fish_ID,     "Fish....: ", self.amounts_of_resources_placed[self.fish_ID + 1])
	print(self.stone_ID,    "Stone...: ", self.amounts_of_resources_placed[self.stone_ID + 1])
	print(self.bison_ID,    "Bison...: ", self.amounts_of_resources_placed[self.bison_ID + 1])
	print(self.hardwood_ID, "Hardwood: ", self.amounts_of_resources_placed[self.hardwood_ID + 1])
	print(self.maize_ID,    "Maize...: ", self.amounts_of_resources_placed[self.maize_ID + 1])
	print(self.lead_ID_ID,    "Maize...: ", self.amounts_of_resources_placed[self.maize_ID + 1])
	print("-");
	print("-----------------------------------------------------");
end

function AssignStartingPlots:GetMajorStrategicResourceQuantityValues()


	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 4, 7, 6, 7, 8;

	if self.resource_setting == 1 or self.resource_setting == 2 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 5, 4, 5, 6;
	elseif self.resource_setting == 3 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 6, 5, 6, 7;
	elseif self.resource_setting == 7 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 5, 8, 7, 8, 9;
	elseif self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 6, 9, 8, 9, 10;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end

function AssignStartingPlots:GetSmallStrategicResourceQuantityValues()

	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 2, 4, 2, 3, 3;

	if self.resource_setting == 1 or self.resource_setting == 2 or self.resource_setting == 3 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 1, 1, 2, 1, 2, 2;
	elseif self.resource_setting == 7 or self.resource_setting == 8 or self.resource_setting == 9 or self.resource_setting == 10 then
		uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = 2, 3, 3, 3, 3, 3;
	end
	return uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt
end

function AssignStartingPlots:PlaceStrategicAndBonusResources()









	local uran_amt, horse_amt, oil_amt, iron_amt, coal_amt, alum_amt = self:GetMajorStrategicResourceQuantityValues()
	

	local bonus_multiplier = 0.65;

	if self.resource_setting == 1 then
		bonus_multiplier = 1;
	elseif self.resource_setting == 2 then
		bonus_multiplier = 0.90;
	elseif self.resource_setting == 3 then
		bonus_multiplier = 0.80;
	elseif self.resource_setting == 4 then
		bonus_multiplier = 0.75;
	elseif self.resource_setting == 6 then
		bonus_multiplier = 0.55;
	elseif self.resource_setting == 7 then
		bonus_multiplier = 0.45;
	elseif self.resource_setting == 8 then
		bonus_multiplier = 0.35;
	elseif self.resource_setting == 9 then
		bonus_multiplier = 0.25;
	elseif self.resource_setting == 10 then
		bonus_multiplier = 0.15;
	end


	print("Map Generation - Placing Strategics");
	local resources_to_place = {
	{self.oil_ID, oil_amt, 65, 1, 4},
	{self.uranium_ID, uran_amt, 35, 1, 4} };
	self:ProcessResourceList(7, 1, self.marsh_list, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 55, 1, 5},
	{self.aluminum_ID, alum_amt, 15, 1, 2},
	{self.iron_ID, iron_amt, 35, 1, 2} };
	self:ProcessResourceList(16, 1, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 65, 1, 5},
	{self.aluminum_ID, alum_amt, 15, 1, 2},
	{self.iron_ID, iron_amt, 20, 1, 2} };
	self:ProcessResourceList(15, 1, self.snow_flat_list, resources_to_place)

	local resources_to_place = {
	{self.oil_ID, oil_amt, 70, 1, 2},
	{self.iron_ID, iron_amt, 30, 1, 2} };
	self:ProcessResourceList(11, 1, self.desert_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.iron_ID, iron_amt, 26, 1, 3},
	{self.coal_ID, coal_amt, 35, 1, 3},
	{self.aluminum_ID, alum_amt, 39, 1, 3} };
	self:ProcessResourceList(22, 1, self.hills_list, resources_to_place)

	local resources_to_place = {
	{self.coal_ID, coal_amt, 30, 1, 2},
	{self.uranium_ID, uran_amt, 70, 1, 2} };
	self:ProcessResourceList(33, 1, self.jungle_flat_list, resources_to_place)
	local resources_to_place = {
	{self.coal_ID, coal_amt, 25, 1, 2},
	{self.oil_ID, oil_amt, 25, 1, 5},
	{self.uranium_ID, uran_amt, 50, 10, 0} };
	self:ProcessResourceList(39, 1, self.forest_flat_list, resources_to_place)

	local resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 5} };
	self:ProcessResourceList(10, 1, self.dry_grass_flat_no_feature, resources_to_place)
	local resources_to_place = {
	{self.horse_ID, horse_amt, 100, 1, 5} };
	self:ProcessResourceList(10, 1, self.plains_flat_no_feature, resources_to_place)

	self:AddModernMinorStrategicsToCityStates()
	
	self:PlaceSmallQuantitiesOfStrategics(23 * bonus_multiplier, self.land_list);
	
	self:PlaceOilInTheSea();

	

	if self.amounts_of_resources_placed[self.iron_ID + 1] < 8 then

		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.iron_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.iron_ID, iron_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.horse_ID + 1] < 4 * self.iNumCivs then
		print("Map has very low horse, adding another.");
		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.plains_flat_no_feature, resources_to_place)
		

		local resources_to_place = { {self.horse_ID, horse_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.dry_grass_flat_no_feature, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 8 then

		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.coal_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.coal_ID, coal_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.oil_ID + 1] < 4 * self.iNumCivs then
		print("Map has very low oil, adding another.");
		local resources_to_place = { {self.oil_ID, oil_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	if self.amounts_of_resources_placed[self.aluminum_ID + 1] < 4 * self.iNumCivs then

		local resources_to_place = { {self.aluminum_ID, alum_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.hills_list, resources_to_place)
	end
	
	while self.amounts_of_resources_placed[self.uranium_ID + 1] < 5 * self.iNumCivs do
		print("Map has very low uranium, adding another.");
		local resources_to_place = { {self.uranium_ID, uran_amt, 100, 0, 0} };
		self:ProcessResourceList(99999, 1, self.land_list, resources_to_place)
	end
	
	

	print("Map Generation - Placing Bonuses");
	
	if self._lek_coastal_refish then
		local iW, iH = Map.GetGridSize()
		local plotDataImmediateCoast = {};
		local plotDataNextToImmediateCoast ={};
		local plotDataIsThreeFromMainland = {};

		plotDataImmediateCoast, plotDataNextToImmediateCoast = GenerateMainlandExpandedCoastData();
		plotDataIsThreeFromMainland = GenerateThreeFromMainlandCoast(plotDataImmediateCoast, plotDataNextToImmediateCoast);

		table.fill(self.mainland_coast_list, false, iW * iH);
		table.fill(self.mainland_coast_list_inner, false, iW * iH);
		table.fill(self.mainland_coast_list_second, false, iW * iH);
		table.fill(self.mainland_coast_list_outer, false, iW * iH);

		local temp_list_pangaea = {};
		local temp_list_pangaea_inner = {};
		local temp_list_pangaea_second = {};
		local temp_list_pangaea_outer = {};


		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				if plotDataImmediateCoast[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_inner[i] = true;
					local test_plot = Map.GetPlot(x, y);
					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_inner, i);
				elseif plotDataNextToImmediateCoast[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_second[i] = true;
					local test_plot = Map.GetPlot(x, y);
					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_second, i);
				elseif plotDataIsThreeFromMainland[i] == true then
					self.mainland_coast_list[i] = true;
					self.mainland_coast_list_outer[i] = true;
					local test_plot = Map.GetPlot(x, y);
					table.insert(temp_list_pangaea, i);
					table.insert(temp_list_pangaea_outer, i);
				end
			end
		end

		local fish_coast_inner = GetShuffledCopyOfTable(temp_list_pangaea_inner)
		local fish_coast_second = GetShuffledCopyOfTable(temp_list_pangaea_second)
		local fish_coast_outer = GetShuffledCopyOfTable(temp_list_pangaea_outer)

		self:PlaceFishMainland(3 * bonus_multiplier, fish_coast_inner);
		self:PlaceFishMainland(8 * bonus_multiplier, fish_coast_second);
		self:PlaceFishMainland(15 * bonus_multiplier, fish_coast_outer);

	elseif self.method == 1 then
		local iW, iH = Map.GetGridSize()
		local plotDataImmediateCoast = {};
		local plotDataNextToImmediateCoast ={};
		local plotDataIsThreeFromMainland = {};

		plotDataImmediateCoast, plotDataNextToImmediateCoast = GenerateMainlandExpandedCoastData();
		plotDataIsThreeFromMainland = GenerateThreeFromMainlandCoast(plotDataImmediateCoast, plotDataNextToImmediateCoast);

		table.fill(self.mainland_coast_list, false, iW * iH);


		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				if plotDataImmediateCoast[i] == true or plotDataNextToImmediateCoast[i] == true or plotDataIsThreeFromMainland[i] == true then
					self.mainland_coast_list[i] = true;
				end
			end
		end

		local temp_list_panagaea = {};

		for x = 0, iW - 1 do
			for y = 0, iH - 1 do
				local i = iW * y + x + 1;
				if self.mainland_coast_list[i] == true then
					local test_plot = Map.GetPlot(x, y);
					table.insert(temp_list_panagaea, i);
				end
			end
		end

		self.coast_list_panagaea = GetShuffledCopyOfTable(temp_list_panagaea)

		self:PlaceFishMainland(10 * bonus_multiplier, self.coast_list_panagaea);
	end

	if self._lek_coastal_refish then

		self:PlaceFish(16 * bonus_multiplier, self.coast_list);
	else
		self:PlaceFish(8 * bonus_multiplier, self.coast_list);
	end


	self:PlaceSexyBonusAtCivStarts()
	self:AddExtraBonusesToHillsRegions()
	
	local resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(6 * bonus_multiplier, 3, self.extra_deer_list, resources_to_place)

	local resources_to_place = {
	{self.wheat_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(6 * bonus_multiplier, 3, self.desert_wheat_list, resources_to_place)

	local resources_to_place = {
	{self.deer_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(8 * bonus_multiplier, 3, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.banana_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(10 * bonus_multiplier, 3, self.banana_list, resources_to_place)

	local resources_to_place = {
	{self.wheat_ID, 1, 100, 1, 3} };
	self:ProcessResourceList(30 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.bison_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(15 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.cow_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.cow_ID, 1, 100, 2, 3} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.bison_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.dry_grass_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.sheep_ID, 1, 100, 1, 1} };
	self:ProcessResourceList(20 * bonus_multiplier, 3, self.hills_open_list, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(10 * bonus_multiplier, 3, self.tundra_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.stone_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(16 * bonus_multiplier, 3, self.desert_flat_no_feature, resources_to_place)

	local resources_to_place = {
	{self.deer_ID, 1, 100, 3, 4} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.forest_flat_that_are_not_tundra, resources_to_place)
	
	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.hills_covered_list, resources_to_place)

	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.flat_covered, resources_to_place)

	local resources_to_place = {
	{self.hardwood_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(22 * bonus_multiplier, 3, self.tundra_flat_forest, resources_to_place)
	
	local resources_to_place = {
	{self.maize_ID, 1, 100, 1, 2} };
	self:ProcessResourceList(35 * bonus_multiplier, 3, self.plains_flat_no_feature, resources_to_place)
	
	
end

function AssignStartingPlots:PlaceResourcesAndCityStates()



















	print("Map Generation - Assigning Luxury Resource Distribution");
	self:AssignLuxuryRoles()
	self:PlaceCityStates()

	self:GenerateGlobalResourcePlotLists()

	print("Map Generation - Placing Luxuries");
	self:PlaceLuxuries()


	self:PlaceStrategicAndBonusResources()
	self:NormalizeCityStateLocations()

	self:FixResourceGraphics()



	Map.RecalculateAreas();


	self:PrintFinalResourceTotalsToLog()

end
