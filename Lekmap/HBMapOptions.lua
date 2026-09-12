


LekmapOptions = {}

function LekmapOptions.NumberValues(first, last, step)
	local values = {}
	for value = first, last, step or 1 do
		values[#values + 1] = tostring(value)
	end
	return values
end


function LekmapOptions.Create(overrides, extraOptions)
	local options = {
		{
			Name = "TXT_KEY_MAP_OPTION_WORLD_AGE",
			Values = {
				"TXT_KEY_MAP_OPTION_THREE_BILLION_YEARS",
				"TXT_KEY_MAP_OPTION_FOUR_BILLION_YEARS",
				"TXT_KEY_MAP_OPTION_FIVE_BILLION_YEARS",
				"No Mountains",
				"TXT_KEY_MAP_OPTION_RANDOM",
			},
			DefaultValue = 2,
			SortPriority = -99,
		},
		{
			Name = "TXT_KEY_MAP_OPTION_TEMPERATURE",
			Values = {
				"TXT_KEY_MAP_OPTION_COOL",
				"TXT_KEY_MAP_OPTION_TEMPERATE",
				"TXT_KEY_MAP_OPTION_HOT",
				"TXT_KEY_MAP_OPTION_RANDOM",
			},
			DefaultValue = 2,
			SortPriority = -98,
		},
		{
			Name = "TXT_KEY_MAP_OPTION_RAINFALL",
			Values = {
				"TXT_KEY_MAP_OPTION_ARID",
				"TXT_KEY_MAP_OPTION_NORMAL",
				"TXT_KEY_MAP_OPTION_WET",
				"TXT_KEY_MAP_OPTION_RANDOM",
			},
			DefaultValue = 2,
			SortPriority = -97,
		},
		{
			Name = "TXT_KEY_MAP_OPTION_SEA_LEVEL",
			Values = {
				"TXT_KEY_MAP_OPTION_LOW",
				"TXT_KEY_MAP_OPTION_MEDIUM",
				"TXT_KEY_MAP_OPTION_HIGH",
				"TXT_KEY_MAP_OPTION_RANDOM",
			},
			DefaultValue = 2,
			SortPriority = -96,
		},
		{
			Name = "Start Quality",
			Values = {
				"Legendary Start - Strat Balance",
				"Legendary - Strat Balance + Uranium",
				"TXT_KEY_MAP_OPTION_STRATEGIC_BALANCE",
				"Strategic Balance With Coal",
				"Strategic Balance With Aluminum",
				"Strategic Balance With Coal & Aluminum",
				"TXT_KEY_MAP_OPTION_RANDOM",
			},
			DefaultValue = 2,
			SortPriority = -95,
		},
		{
			Name = "Start Distance",
			Values = {
				"Close",
				"Normal",
				"Far - Warning: May sometimes crash during map generation",
			},
			DefaultValue = 2,
			SortPriority = -94,
		},
		{
			Name = "Natural Wonders",
			Values = {
				"0",
				"1",
				"2",
				"3",
				"4",
				"5",
				"6",
				"7",
				"8",
				"9",
				"10",
				"11",
				"12",
				"Random",
				"Default",
			},
			DefaultValue = 15,
			SortPriority = -93,
		},
		{
			Name = "Grass Moisture",
			Values = {
				"Wet",
				"Normal",
				"Dry",
			},
			DefaultValue = 2,
			SortPriority = -92,
		},
		{
			Name = "Rivers",
			Values = {
				"Sparse",
				"Average",
				"Plentiful",
			},
			DefaultValue = 2,
			SortPriority = -91,
		},
		{
			Name = "Tundra",
			Values = {
				"Sparse",
				"Average",
				"Plentiful",
			},
			DefaultValue = 2,
			SortPriority = -90,
		},
		{
			Name = "Land Size X",
			Values = LekmapOptions.NumberValues(30, 110, 2),
			DefaultValue = 17,
			SortPriority = -89,
		},
		{
			Name = "Land Size Y",
			Values = LekmapOptions.NumberValues(20, 76, 2),
			DefaultValue = 16,
			SortPriority = -88,
		},
		{
			Name = "TXT_KEY_MAP_OPTION_RESOURCES",
			Values = {
				"1 -- Nearly Nothing",
				"2",
				"3",
				"4",
				"5 -- Default",
				"6",
				"7",
				"8",
				"9",
				"10 -- Almost no normal tiles left",
			},
			DefaultValue = 5,
			SortPriority = -87,
		},
		{
			Name = "Balanced Regionals",
			Values = {
				"Yes",
				"No",
			},
			DefaultValue = 1,
			SortPriority = -90,
		},
	}
	for index, override in pairs(overrides or {}) do
		for key, value in pairs(override) do
			options[index][key] = value
		end
	end
	for _, option in ipairs(extraOptions or {}) do
		options[#options + 1] = option
	end
	return options
end
