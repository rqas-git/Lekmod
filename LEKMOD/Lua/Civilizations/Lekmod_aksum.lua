

include("Lekmod_utilities.lua")
include("PlotIterators")

local this_civ = GameInfoTypes["CIVILIZATION_AKSUM"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)




function lekmod_aksum_ua_follow_religion(player_id, founded_city, religion_id)


   local player = Players[player_id]
   if not player:IsAlive() or player:GetCivilizationType() ~= this_civ then return end
   for city in player:Cities() do
      if city and city:GetID() ~= founded_city then
            city:AdoptReligionFully(religion_id)
      end
   end

end



function lekmod_aksum_ui_faith_on_heal(player_id, unit_id, _, x, y)

   print("Aksum UI")
   local player = Players[player_id]

   local unit = player:GetUnitByID(unit_id)
   local needs_same_player = false
   print(GameInfoTypes["IMPROVEMENT_AKSUM"])
   if unit:IsNearImprovementType(GameInfoTypes["IMPROVEMENT_AKSUM"], 1, needs_same_player) then

      print("Unit healed next to Aksum UI")
      local faith = 2
      player:ChangeFaith(faith)

      local unit_plot = Map.GetPlot(x, y)


      if player_id == Game.GetActivePlayer() then
         local hex_position = ToHexFromGrid{x = unit_plot:GetX(), y = unit_plot:GetY()}
         local world_position = HexToWorld(hex_position)
         Events.AddPopupTextEvent(world_position, "+2 [ICON_PEACE] Faith")
      end

   end


end

if is_active then
	GameEvents.ReligionFounded.Add(lekmod_aksum_ua_follow_religion)
end

GameEvents.UnitHealed.Add(lekmod_aksum_ui_faith_on_heal)
