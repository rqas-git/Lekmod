
from pathlib import Path
import sys
import shutil

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "LekmodInstaller"))
from ui_assets import configure_ui


def restore_stock_city_banners(destination):
    """Use stock banner controls with grid-based Aspyr world positioning."""
    source = destination / 'Lua/tmp/ui/CityBanners'
    ui = destination / 'Lua/UI'
    for name in ('CityBannerManager.lua', 'CityBannerManager.xml'):
        packaged = source / f'{name}.ignore'
        if not packaged.is_file():
            raise RuntimeError(f'Missing stock city-banner asset: {packaged}')
        shutil.copy2(packaged, ui / name)

    banner = ui / 'CityBannerManager.lua'
    contents = banner.read_text()
    replacements = (
        ('local WorldPositionOffset = { x = 0, y = 0, z = 35 };',
         '''local WorldPositionOffset = { x = 0, y = 0, z = 35 };

local function SetAnchorAtGrid( anchor, gridX, gridY, offsetZ )
\tlocal worldX, worldY, worldZ = GridToWorld( gridX, gridY );
\tanchor:SetWorldPositionVal( worldX, worldY, worldZ + offsetZ );
end'''),
        ('''    local gridPosX, gridPosY = ToGridFromHex( hexPos.x, hexPos.y );
\t\t
\tlocal isActiveType''',
         '''    local city = Players[playerID] and Players[playerID]:GetCityByID(cityID);
    if city == nil then return; end
    local gridPosX, gridPosY = city:GetX(), city:GetY();
\t\t
\tlocal isActiveType'''),
        ('svStrikeButton.Anchor:SetWorldPosition( HexToWorld(hexPos) );',
         'SetAnchorAtGrid( svStrikeButton.Anchor, gridPosX, gridPosY, 0 );'),
        ('''local HexPos = HexToWorld( hexPos );
\tcontrolTable.Anchor:SetWorldPosition( VecAdd( HexPos, WorldPositionOffset ) );''',
         'SetAnchorAtGrid( controlTable.Anchor, gridPosX, gridPosY, WorldPositionOffset.z );'),
        ('''local gridPosX, gridPosY = ToGridFromHex( instance.Hex.x, instance.Hex.y );
\t\tlocal worldPos = HexToWorld( instance.Hex );''',
         '''local city = Players[instance.playerID]:GetCityByID(instance.cityID);
\t\tif city == nil then return; end
\t\tlocal gridPosX, gridPosY = city:GetX(), city:GetY();'''),
        ('svStrikeButton.Anchor:SetWorldPosition( worldPos );',
         'SetAnchorAtGrid( svStrikeButton.Anchor, gridPosX, gridPosY, 0 );'),
        ('controlTable.Anchor:SetWorldPosition( VecAdd( worldPos, WorldPositionOffset ) );',
         'SetAnchorAtGrid( controlTable.Anchor, gridPosX, gridPosY, WorldPositionOffset.z );'),
    )
    for original, replacement in replacements:
        if contents.count(original) != 1:
            raise RuntimeError('Mac city-banner position patch no longer matches the stock UI.')
        contents = contents.replace(original, replacement)
    banner.write_text(contents)


def prepare_lekmod(source, destination, eui=None):
    shutil.copytree(source, destination,
                    ignore=lambda _path, names: [n for n in names
                        if n.startswith('.') or Path(n).suffix.lower() in ('.dll', '.pdb', '.bat')])
    configure_ui(destination, want_eui=eui is not None, eui_folder=eui, preserve_all=False)
    if eui is not None:
        # EUI's banner controls do not render in Aspyr's macOS build. Stock
        # controls work, but use event hexes that misplace banners on Lekmap;
        # restore them with positions derived from authoritative plot grids.
        restore_stock_city_banners(destination)
    if eui is not None and (eui / 'Core/CityStateStatusHelper.lua').is_file():
        shutil.copy2(destination / 'Lua/tmp/eui/Core/CityStateStatusHelper.lua.ignore',
                     destination / 'Lua/UI/CityStateStatusHelper.lua')

    version = destination / 'Lua/Utilities/Lekmod_version.lua'
    original = 'return Network ~= nil and type(Network.HttpRequest) == "function"'
    contents = version.read_text()
    if contents.count(original) != 1:
        raise RuntimeError('Mac HTTP compatibility patch no longer matches the version helper.')
    version.write_text(contents.replace(
        original, 'return false'))


def prepare_lekmap(source, destination):
    if not any(source.glob('Lekmap*.lua')):
        raise RuntimeError(f'No Lekmap scripts found in {source}')
    shutil.copytree(source, destination, ignore=shutil.ignore_patterns('.*'))
