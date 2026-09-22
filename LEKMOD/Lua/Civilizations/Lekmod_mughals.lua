

include("Lekmod_utilities.lua")
include("PlotIterators.lua")

local this_civ = GameInfoTypes["CIVILIZATION_MUGHALS"]
local is_active = LekmodUtilities:is_civilization_active(this_civ)

local DUMMY_BUILDING = "BUILDING_DUMMY_MUGHALS"






function lekmod_ua_mughals_foreign_religion_check(player_id)
   local player = Players[player_id]


   if not player or not player:IsAlive() or player:GetCivilizationType() ~= this_civ then
      return
   end

   local dummy_building_id = GameInfoTypes[DUMMY_BUILDING]
   if not dummy_building_id then
      print("Error: Could not find " .. DUMMY_BUILDING)
      return
   end


   local mughal_religion = -1
   if player:HasCreatedReligion() then
      mughal_religion = player:GetReligionCreatedByPlayer()
   end


   for city in player:Cities() do
      if city:IsHasBuilding(dummy_building_id) then
         city:SetNumRealBuilding(dummy_building_id, 0)
      end
   end


   for city in player:Cities() do
      local city_religion = city:GetReligiousMajority()


      if city_religion ~= ReligionTypes.RELIGION_PANTHEON and city_religion ~= -1 and city_religion ~= mughal_religion then


         city:SetNumRealBuilding(dummy_building_id, 1)


         for other_player_id = 0, GameDefines.MAX_MAJOR_CIVS - 1 do
            local other_player = Players[other_player_id]
            

            if other_player and other_player:IsAlive() and other_player_id ~= player_id then
               
               for other_city in other_player:Cities() do

                  if other_city:IsHolyCityForReligion(city_religion) then

                     if not other_city:IsHasBuilding(dummy_building_id) then
                        other_city:SetNumRealBuilding(dummy_building_id, 1)
                     end
                  end
               end
            end
         end
      end
   end
end


function lekmod_ua_mughals_religion_changed(player_id, city_id, religion_id, majority)
   lekmod_ua_mughals_foreign_religion_check(player_id)
end


function lekmod_ua_mughals_turn_start(player_id)
   lekmod_ua_mughals_foreign_religion_check(player_id)
end


function lekmod_ua_mughals_city_acquired(old_owner_id, new_owner_id, city_id)
   lekmod_ua_mughals_foreign_religion_check(new_owner_id)
end


if is_active then
   GameEvents.PlayerDoTurn.Add(lekmod_ua_mughals_turn_start)
   GameEvents.CityReligionChanged.Add(lekmod_ua_mughals_religion_changed)
   GameEvents.CityCaptureComplete.Add(lekmod_ua_mughals_city_acquired)
   GameEvents.PlayerCityFounded.Add(function(player_id) lekmod_ua_mughals_foreign_religion_check(player_id) end)
end
