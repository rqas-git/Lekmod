


























SECTOR_NORTH = 1
SECTOR_NORTHEAST = 2
SECTOR_SOUTHEAST = 3
SECTOR_SOUTH = 4
SECTOR_SOUTHWEST = 5
SECTOR_NORTHWEST = 6

DIRECTION_CLOCKWISE = false
DIRECTION_ANTICLOCKWISE = true

DIRECTION_OUTWARDS = false
DIRECTION_INWARDS = true

CENTRE_INCLUDE = true
CENTRE_EXCLUDE = false

function PlotRingIterator(pPlot, r, sector, anticlock)





  if (pPlot ~= nil and r > 0) then
    local hex = ToHexFromGrid({x=pPlot:GetX(), y=pPlot:GetY()})
    local x, y = hex.x, hex.y


    local function north(x, y, r, i) return {x=x-r+i, y=y+r} end

    local function northeast(x, y, r, i) return {x=x+i, y=y+r-i} end

    local function southeast(x, y, r, i) return {x=x+r, y=y-i} end

    local function south(x, y, r, i) return {x=x+r-i, y=y-r} end

    local function southwest(x, y, r, i) return {x=x-i, y=y-r+i} end

    local function northwest(x, y, r, i) return {x=x-r, y=y+i} end

    local side = {north, northeast, southeast, south, southwest, northwest}
    if (sector) then
      for i=(anticlock and 1 or 2), sector, 1 do
        table.insert(side, table.remove(side, 1))
      end
    end


    local next = coroutine.create(function ()
      if (anticlock) then
        for s=6, 1, -1 do
          for i=r, 1, -1 do
            coroutine.yield(side[s](x, y, r, i))
          end
        end
      else
        for s=1, 6, 1 do
          for i=0, r-1, 1 do
            coroutine.yield(side[s](x, y, r, i))
          end
        end
      end

      return nil
    end)


    return function ()
      local pEdgePlot = nil
      local success, hex = coroutine.resume(next)


      while (success and hex ~= nil and pEdgePlot == nil) do
        pEdgePlot = Map.GetPlot(ToGridFromHex(hex.x, hex.y))
        if (pEdgePlot == nil) then success, hex = coroutine.resume(next) end
      end

      return success and pEdgePlot or nil
    end
  else

    return function () return nil end
  end
end


function PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre)


  local next = coroutine.create(function ()
    if (centre and not inwards) then
      coroutine.yield(pPlot)
    end

    if (inwards) then
      for i=r, 1, -1 do
        for pEdgePlot in PlotRingIterator(pPlot, i, sector, anticlock) do
          coroutine.yield(pEdgePlot)
        end
      end
    else
      for i=1, r, 1 do
        for pEdgePlot in PlotRingIterator(pPlot, i, sector, anticlock) do
          coroutine.yield(pEdgePlot)
        end
      end
    end

    if (centre and inwards) then
      coroutine.yield(pPlot)
    end

    return nil
  end)


  return function ()
    local success, pAreaPlot = coroutine.resume(next)
    return success and pAreaPlot or nil
  end
end


function PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre)
  print(string.format("PlotAreaSweepIterator((%i, %i), r=%i, s=%i, d=%s, w=%s, c=%s)", pPlot:GetX(), pPlot:GetY(), r, (sector or SECTOR_NORTH), (anticlock and "rev" or "fwd"), (inwards and "in" or "out"), (centre and "yes" or "no")))

  local next = coroutine.create(function ()
    if (centre and not inwards) then
      coroutine.yield(pPlot)
    end

    local iterators = {}
    for i=1, r, 1 do
      iterators[i] = PlotRingIterator(pPlot, i, sector, anticlock)
    end

    for s=1, 6, 1 do



      if (inwards) then
        for i=r, 1, -1 do
          for j=r, i, -1 do
            coroutine.yield(iterators[j]())
          end
        end
      else
        for i=1, r, 1 do
          for j=i, r, 1 do
            coroutine.yield(iterators[j]())
          end
        end
      end
    end

    if (centre and inwards) then
      coroutine.yield(pPlot)
    end

    return nil
  end)


  return function ()
    local success, pAreaPlot = coroutine.resume(next)
    return success and pAreaPlot or nil
  end
end






highlights = {
  RED     = {x=1.0, y=0.0, z=0.0, w=1.0},
  GREEN   = {x=0.0, y=1.0, z=0.0, w=1.0},
  BLUE    = {x=0.0, y=0.0, z=1.0, w=1.0},
  CYAN    = {x=0.0, y=1.0, z=1.0, w=1.0},
  YELLOW  = {x=1.0, y=1.0, z=0.0, w=1.0},
  MAGENTA = {x=1.0, y=0.0, z=1.0, w=1.0},
  BLACK   = {x=0.5, y=0.5, z=0.5, w=1.0}
}

function TestPlotHighlight(pPlot, highlight)
  print(pPlot:GetX(), pPlot:GetY())
  if (highlight ~= nil) then
    Events.SerialEventHexHighlight(ToHexFromGrid(Vector2(pPlot:GetX(), pPlot:GetY())), true, highlight)
  end
end

function TestPlotRingIterator(pPlot, r, sector, anticlock, highlight)
  for pEdgePlot in PlotRingIterator(pPlot, r, sector, anticlock) do
    TestPlotHighlight(pEdgePlot, highlight)
  end
end

function TestPlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre, highlight)
  for pAreaPlot in PlotAreaSpiralIterator(pPlot, r, sector, anticlock, inwards, centre) do
    TestPlotHighlight(pAreaPlot, highlight)
  end
end

function TestPlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre, highlight)
  for pAreaPlot in PlotAreaSweepIterator(pPlot, r, sector, anticlock, inwards, centre) do
    TestPlotHighlight(pAreaPlot, highlight)
  end
end
