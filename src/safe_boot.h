#pragma once

/**
 * @brief Boot savely to always enable ota updates
 * Checks if the esp has crashed and if it did it will not boot the app,
 * but instead just connect to wifi and enable ota updates
 * 
 */
void safeBoot();