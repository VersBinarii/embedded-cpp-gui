/*
 * point.h
 *
 *  Created on: Apr 13, 2021
 *      Author: kris
 */

#ifndef INC_GRAPHICS_POINT_H_
#define INC_GRAPHICS_POINT_H_

template <typename T> struct Point {
        T x;
        T y;
        constexpr Point () : x (0), y (0) {}
        constexpr Point (T x, T y) : x (x), y (y) {}
    };

#endif /* INC_GRAPHICS_POINT_H_ */
