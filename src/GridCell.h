#ifndef VALIDATIEAPP_GRIDCELL_H
#define VALIDATIEAPP_GRIDCELL_H

enum class CellType {
    Empty,
    Ground,
    Water, WaterLeft, WaterRight,           // Deadly for Fireboy, safe for Watergirl
    Fire, FireLeft, FireRight,              // Deadly for Watergirl, safe for Fireboy
    Poison, PoisonLeft, PoisonRight,        // Deadly for BOTH characters
    RedDoor,                                // Exit for Fireboy
    BlueDoor,                               // Exit for Watergirl
    Button,                                 // Pressure plate/switch maybe make this for blue and red too
    DiamondBlue, DiamondRed,                // Collectible
    SlopeLeft,                              // Slope
    SlopeRight                              // Slope
};

// Character types for hazard checking
enum class CharacterType {
    Fireboy,
    Watergirl
};

#endif //VALIDATIEAPP_GRIDCELL_H