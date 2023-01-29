#include <iostream>
#include <fstream>

#define OLC_PGE_APPLICATION
#include "libs/olcPixelGameEngine.hpp"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "libs/olcPGEX_View.hpp"
#include "libs/json.hpp"

class Game : public olc::PixelGameEngine
{
private:
    std::string resourcePath = "resources/";
    std::ifstream map;
    olc::Sprite *mapSprite;
    olc::Decal *mapDecal;

    olc::TransformedView tv;

    std::vector<int> tiles;
    int columns;
    int rows;
    int tileSize;
    int textureRows;
    int textureColumns;

    olc::vf2d playerPos = {128.0f, 120.0f};

public:
    Game(){ sAppName = "JSON Tiled Test"; }

    bool OnUserCreate() override
    {
        tv = olc::TransformedView();

        map.open(resourcePath+"Map.json");
        nlohmann::json parsedMap = nlohmann::json::parse(map);

        std::string texturePath = parsedMap.at("layers")[0].at("properties")[0].at("value");
        mapSprite = new olc::Sprite( resourcePath + texturePath );
        mapDecal = new olc::Decal(mapSprite);

        columns = parsedMap.at("layers")[0].at("height");
        rows = parsedMap.at("layers")[0].at("width");
        tileSize = parsedMap.at("layers")[0].at("properties")[1].at("value");

        textureRows = mapDecal->sprite->width / tileSize;
        textureColumns = mapDecal->sprite->height / tileSize;

        // std::cout << textureColumns << " " << textureRows << '\n';

        for ( unsigned int i{}; i < parsedMap.at("layers")[0].at("data").size(); ++i)
        {
            tiles.push_back(parsedMap.at("layers")[0].at("data")[i]);
            // std::cout << tiles[i] << " ";
            // std::cout << '\n';
        }

        return 1;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        if (GetKey(olc::Key::W).bHeld) playerPos.y -= 0.1;
        if (GetKey(olc::Key::A).bHeld) playerPos.x -= 0.1;
        if (GetKey(olc::Key::S).bHeld) playerPos.y += 0.1;
        if (GetKey(olc::Key::D).bHeld) playerPos.x += 0.1;

        tv.SetWorldOffset((olc::vi2d){(int)(playerPos.x - (ScreenWidth()/2)), (int)(playerPos.y - (ScreenHeight()/2))});


        Clear(olc::GREY);

        if (mapDecal)
        {

            for (int column{}; column < columns; ++column)
            {



                
                for (int row{}; row < rows; ++row)
                {
                    int tileData = column * rows + row;
                    int tileDraw = tiles[tileData];

                    // std::cout << tileDraw << " ";

                    if (tileDraw != 0)
                    {
                        int tempColumn = (tileDraw -1) / textureRows;
                        int tempRow = tileDraw - (tempColumn * textureRows);

                        
                        tv.DrawPartialDecal(
                            {(float)(row * tileSize), (float)(column * tileSize)}, // position on screen
                            {(float)tileSize, (float)tileSize}, // size on screen
                            mapDecal, // decal
                            {(float)((tempRow -1) * tileSize) + 0.01f, (float)(tempColumn * tileSize) + 0.01f}, //position in texture
                            {(float)tileSize - 0.02f, (float)tileSize - 0.02f}); //size in texture


                        // tv.DrawCircle(playerPos, 5, olc::GREEN);

                    }
                    // else std::cout << "x" << " ";
                }
                // std::cout << '\n';
            }
        }

        // std::cout << '\n';
        
        return 1;
    }

    bool OnUserDestroy() override
    {
        if (mapDecal)
        {
            delete mapSprite;
            delete mapDecal;
        }

        return 1;
    }
};

int main()
{
    Game game;
    if (game.Construct(256, 240, 6, 6))
        game.Start();

    return 0;
}