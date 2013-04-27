#ifndef GLOBALS_H
#define GLOBALS_H

    // 50px per 1 meter. so that box2d has a range of entity sizes between 0.1 and 10 meters.
    static constexpr double PIXELS_PER_METER = 50;
    static constexpr float BLOCK_SIZE = 16.0f / PIXELS_PER_METER;

    //FIXME: find good values for this. Basically this is as far out as *anyone* can see when they zoom out, regardless of resolution they will all only be able to see
    // this many blocks out.
    // units are just tile counts, not in meters, just number of tiles.
    static constexpr float MAX_VIEWPORT_WIDTH = 200.0f;
    static constexpr float MAX_VIEWPORT_HEIGHT = 200.0f;

    /// ACTIVECHUNK_SIZE^2 == total tiles within chunk
    static constexpr uint32_t ACTIVECHUNK_SIZE = 64;

#endif
