// SPDX-License-Identifier: GPL-2.0-or-later
/** @file
 * Test for SVG colors
 *//*
 * Authors: see git history
 *
 * Copyright (C) 2010 Authors
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */
#include <2geom/coord.h>
#include <2geom/curves.h>
#include <2geom/pathvector.h>
#include <glib.h>
#include <gtest/gtest.h>
#include <vector>

#include "preferences.h"
#include "svg/svg.h"
#include "helper/geom.h"

class SvgPathGeomTest : public ::testing::Test
{
public:
    std::vector<std::string> rectanglesAbsoluteClosed = {"M 1,2 L 4,2 L 4,8 L 1,8 z", "M 1,2 4,2 4,8 1,8 z",
                                                         "M 1,2 H 4 V 8 H 1 z"};
    std::vector<std::string> rectanglesRelativeClosed = {"m 1,2 l 3,0 l 0,6 l -3,0 z", "m 1,2 3,0 0,6 -3,0 z",
                                                         "m 1,2 h 3 v 6 h -3 z"};
    std::vector<std::string> rectanglesAbsoluteOpen = {"M 1,2 L 4,2 L 4,8 L 1,8 L 1,2", "M 1,2 4,2 4,8 1,8 1,2",
                                                       "M 1,2 H 4 V 8 H 1 V 2"};
    std::vector<std::string> rectanglesRelativeOpen = {"m 1,2 l 3,0 l 0,6 l -3,0 l 0,-6", "m 1,2 3,0 0,6 -3,0 0,-6",
                                                       "m 1,2 h 3 v 6 h -3 v -6"};
    std::vector<std::string> rectanglesAbsoluteClosed2 = {"M 1,2 L 4,2 L 4,8 L 1,8 L 1,2 z", "M 1,2 4,2 4,8 1,8 1,2 z",
                                                          "M 1,2 H 4 V 8 H 1 V 2 z"};
    std::vector<std::string> rectanglesRelativeClosed2{"m 1,2 l 3,0 l 0,6 l -3,0 l 0,-6 z", "m 1,2 3,0 0,6 -3,0 0,-6 z",
                                                       "m 1,2 h 3 v 6 h -3 v -6 z"};
    Geom::PathVector rectanglepvopen;
    Geom::PathVector rectanglepvclosed;
    Geom::PathVector rectanglepvclosed2;

    void SetUp() override
    {
        rectanglepvopen.clear();
        rectanglepvopen.push_back(Geom::Path(Geom::Point(1, 2)));
        rectanglepvopen.back().append(Geom::LineSegment(Geom::Point(1, 2), Geom::Point(4, 2)));
        rectanglepvopen.back().append(Geom::LineSegment(Geom::Point(4, 2), Geom::Point(4, 8)));
        rectanglepvopen.back().append(Geom::LineSegment(Geom::Point(4, 8), Geom::Point(1, 8)));
        rectanglepvopen.back().append(Geom::LineSegment(Geom::Point(1, 8), Geom::Point(1, 2)));
        rectanglepvclosed.clear();
        rectanglepvclosed.push_back(Geom::Path(Geom::Point(1, 2)));
        rectanglepvclosed.back().append(Geom::LineSegment(Geom::Point(1, 2), Geom::Point(4, 2)));
        rectanglepvclosed.back().append(Geom::LineSegment(Geom::Point(4, 2), Geom::Point(4, 8)));
        rectanglepvclosed.back().append(Geom::LineSegment(Geom::Point(4, 8), Geom::Point(1, 8)));
        rectanglepvclosed.back().close();
        rectanglepvclosed2.clear();
        rectanglepvclosed2.push_back(Geom::Path(Geom::Point(1, 2)));
        rectanglepvclosed2.back().append(Geom::LineSegment(Geom::Point(1, 2), Geom::Point(4, 2)));
        rectanglepvclosed2.back().append(Geom::LineSegment(Geom::Point(4, 2), Geom::Point(4, 8)));
        rectanglepvclosed2.back().append(Geom::LineSegment(Geom::Point(4, 8), Geom::Point(1, 8)));
        rectanglepvclosed2.back().append(Geom::LineSegment(Geom::Point(1, 8), Geom::Point(1, 2)));
        rectanglepvclosed2.back().close();
    }

    bool bpathEqual(Geom::PathVector const &a, Geom::PathVector const &b, double eps = 1e-16)
    {
        if (a.size() != b.size()) {
            printf("PathVectors not the same size: %u != %u", static_cast<unsigned int>(a.size()),
                   static_cast<unsigned int>(b.size()));
            return false;
        }
        for (size_t i = 0; i < a.size(); i++) {
            Geom::Path const &pa = a[i];
            Geom::Path const &pb = b[i];
            if (pa.closed() && !pb.closed()) {
                printf("Left subpath is closed, right subpath is open. Subpath: %u", static_cast<unsigned int>(i));
                return false;
            }
            if (!pa.closed() && pb.closed()) {
                printf("Right subpath is closed, left subpath is open. Subpath: %u", static_cast<unsigned int>(i));
                return false;
            }
            if (pa.size() != pb.size()) {
                printf("Not the same number of segments: %u != %u, subpath: %u", static_cast<unsigned int>(pa.size()),
                       static_cast<unsigned int>(pb.size()), static_cast<unsigned int>(i));
                return false;
            }
            for (size_t j = 0; j < pa.size(); j++) {
                Geom::Curve const *ca = &pa[j];
                Geom::Curve const *cb = &pb[j];
                if (typeid(*ca) == typeid(*cb)) {
                    if (Geom::LineSegment const *la = dynamic_cast<Geom::LineSegment const *>(ca)) {
                        Geom::LineSegment const *lb = dynamic_cast<Geom::LineSegment const *>(cb);
                        if (!Geom::are_near((*la)[0], (*lb)[0], eps)) {
                            printf("Different start of segment: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[0][Geom::X], (*la)[0][Geom::Y], (*lb)[0][Geom::X], (*lb)[0][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                        if (!Geom::are_near((*la)[1], (*lb)[1], eps)) {
                            printf("Different end of segment: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[1][Geom::X], (*la)[1][Geom::Y], (*lb)[1][Geom::X], (*lb)[1][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                    } else if (Geom::CubicBezier const *la = dynamic_cast<Geom::CubicBezier const *>(ca)) {
                        Geom::CubicBezier const *lb = dynamic_cast<Geom::CubicBezier const *>(cb);
                        if (!Geom::are_near((*la)[0], (*lb)[0], eps)) {
                            printf("Different start of segment: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[0][Geom::X], (*la)[0][Geom::Y], (*lb)[0][Geom::X], (*lb)[0][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                        if (!Geom::are_near((*la)[1], (*lb)[1], eps)) {
                            printf("Different 1st control point: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[1][Geom::X], (*la)[1][Geom::Y], (*lb)[1][Geom::X], (*lb)[1][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                        if (!Geom::are_near((*la)[2], (*lb)[2], eps)) {
                            printf("Different 2nd control point: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[2][Geom::X], (*la)[2][Geom::Y], (*lb)[2][Geom::X], (*lb)[2][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                        if (!Geom::are_near((*la)[3], (*lb)[3], eps)) {
                            printf("Different end of segment: (%g,%g) != (%g,%g), subpath: %u, segment: %u",
                                   (*la)[3][Geom::X], (*la)[3][Geom::Y], (*lb)[3][Geom::X], (*lb)[3][Geom::Y],
                                   static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                            return false;
                        }
                    } else {
                        printf("Unknown curve type: %s, subpath: %u, segment: %u", typeid(*ca).name(),
                               static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                        return false;
                    }
                } else // not same type
                {
                    printf("Different curve types: %s != %s, subpath: %u, segment: %u", typeid(*ca).name(),
                           typeid(*cb).name(), static_cast<unsigned int>(i), static_cast<unsigned int>(j));
                    return false;
                }
            }
        }
        return true;
    }
};

TEST_F(SvgPathGeomTest, testReadRectanglesAbsoluteClosed)
{
    for (size_t i = 0; i < rectanglesAbsoluteClosed.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesAbsoluteClosed[i].c_str());
        EXPECT_TRUE(bpathEqual(pv, rectanglepvclosed)) << rectanglesAbsoluteClosed[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadRectanglesRelativeClosed)
{
    for (size_t i = 0; i < rectanglesRelativeClosed.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesRelativeClosed[i].c_str());
        ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << rectanglesRelativeClosed[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadRectanglesAbsoluteOpen)
{
    for (size_t i = 0; i < rectanglesAbsoluteOpen.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesAbsoluteOpen[i].c_str());
        ASSERT_TRUE(bpathEqual(pv, rectanglepvopen)) << rectanglesAbsoluteOpen[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadRectanglesRelativeOpen)
{
    for (size_t i = 0; i < rectanglesRelativeOpen.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesRelativeOpen[i].c_str());
        ASSERT_TRUE(bpathEqual(pv, rectanglepvopen)) << rectanglesRelativeOpen[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadRectanglesAbsoluteClosed2)
{
    for (size_t i = 0; i < rectanglesAbsoluteClosed2.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesAbsoluteClosed2[i].c_str());
        ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed2)) << rectanglesAbsoluteClosed2[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadRectanglesRelativeClosed2)
{
    for (size_t i = 0; i < rectanglesRelativeClosed2.size(); i++) {
        Geom::PathVector pv = sp_svg_read_pathv(rectanglesRelativeClosed2[i].c_str());
        ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed2)) << rectanglesRelativeClosed2[i].c_str();
    }
}

TEST_F(SvgPathGeomTest, testReadConcatenatedPaths)
{
    // Note that finalPoint doesn't actually return the final point of the path, just the last given point... (but since
    // this might be intentional and we're not testing lib2geom here, we just specify the final point explicitly
    Geom::PathVector pv_good;
    pv_good.push_back(rectanglepvclosed.back());
    pv_good.push_back(rectanglepvopen.back() * Geom::Translate(1, 2) /* * Geom::Translate(pv_good[0].finalPoint())*/);
    pv_good.push_back(rectanglepvclosed.back() * Geom::Translate(2, 4) /* *Geom::Translate(pv_good[1].finalPoint())*/);
    pv_good.push_back(rectanglepvopen.back());
    pv_good[0].close();
    pv_good[1].close(false);
    pv_good[2].close();
    pv_good[3].close(false);
    std::string path_str = rectanglesAbsoluteClosed[0] + rectanglesRelativeOpen[0] + rectanglesRelativeClosed[0] +
                           rectanglesAbsoluteOpen[0];
    Geom::PathVector pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, pv_good));
}

TEST_F(SvgPathGeomTest, testReadZeroLengthSubpaths)
{
    // Per the SVG 1.1 specification (section F5) zero-length subpaths are relevant
    Geom::PathVector pv_good;
    pv_good.push_back(Geom::Path(Geom::Point(0, 0)));
    pv_good.push_back(Geom::Path(Geom::Point(1, 1)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(1, 1), Geom::Point(2, 2)));
    pv_good.push_back(Geom::Path(Geom::Point(3, 3)));
    pv_good.back().close();
    pv_good.push_back(Geom::Path(Geom::Point(4, 4)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(4, 4), Geom::Point(5, 5)));
    pv_good.back().close();
    pv_good.push_back(Geom::Path(Geom::Point(6, 6)));
    { // Test absolute version
        char const *path_str = "M 0,0 M 1,1 L 2,2 M 3,3 z M 4,4 L 5,5 z M 6,6";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
    { // Test relative version
        char const *path_str = "m 0,0 m 1,1 l 1,1 m 1,1 z m 1,1 l 1,1 z m 2,2";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
}

TEST_F(SvgPathGeomTest, testReadImplicitMoveto)
{
    g_warning("Currently lib2geom (/libnr) has no way of specifying the difference between 'M 0,0 ... z M 0,0 L 1,0' "
              "and 'M 0,0 ... z L 1,0', the SVG specification does state that these should be handled differently with "
              "respect to markers however, see the description of the 'orient' attribute of the 'marker' element.");
    Geom::PathVector pv_good;
    pv_good.push_back(Geom::Path(Geom::Point(1, 1)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(1, 1), Geom::Point(2, 2)));
    pv_good.back().close();
    pv_good.push_back(Geom::Path(Geom::Point(1, 1)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(1, 1), Geom::Point(3, 3)));
    pv_good.back().close();
    { // Test absolute version
        char const *path_str = "M 1,1 L 2,2 z L 3,3 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
    { // Test relative version
        char const *path_str = "M 1,1 l 1,1 z l 2,2 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
}

TEST_F(SvgPathGeomTest, testReadFloatingPoint)
{
    Geom::PathVector pv_good1;
    pv_good1.push_back(Geom::Path(Geom::Point(.01, .02)));
    pv_good1.back().append(Geom::LineSegment(Geom::Point(.01, .02), Geom::Point(.04, .02)));
    pv_good1.back().append(Geom::LineSegment(Geom::Point(.04, .02), Geom::Point(1.5, 1.6)));
    pv_good1.back().append(Geom::LineSegment(Geom::Point(1.5, 1.6), Geom::Point(.01, .08)));
    pv_good1.back().append(Geom::LineSegment(Geom::Point(.01, .08), Geom::Point(.01, .02)));
    pv_good1.back().close();
    { // Test decimals
        char const *path_str = "M .01,.02 L.04.02 L1.5,1.6L0.01,0.08 .01.02 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good1)) << path_str;
    }
    Geom::PathVector pv_good2;
    pv_good2.push_back(Geom::Path(Geom::Point(.01, .02)));
    pv_good2.back().append(Geom::LineSegment(Geom::Point(.01, .02), Geom::Point(.04, .02)));
    pv_good2.back().append(Geom::LineSegment(Geom::Point(.04, .02), Geom::Point(1.5, 1.6)));
    pv_good2.back().append(Geom::LineSegment(Geom::Point(1.5, 1.6), Geom::Point(.01, .08)));
    pv_good2.back().close();
    { // Test exponent
        char const *path_str = "M 1e-2,.2e-1 L 0.004e1,0.0002e+2 L0150E-2,1.6e0L1.0e-2,80e-3 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good2)) << path_str;
    }
}

TEST_F(SvgPathGeomTest, testReadImplicitSeparation)
{
    // Coordinates need not be separated by whitespace if they can still be read unambiguously
    Geom::PathVector pv_good;
    pv_good.push_back(Geom::Path(Geom::Point(.1, .2)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(.1, .2), Geom::Point(.4, .2)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(.4, .2), Geom::Point(.4, .8)));
    pv_good.back().append(Geom::LineSegment(Geom::Point(.4, .8), Geom::Point(.1, .8)));
    pv_good.back().close();
    { // Test absolute
        char const *path_str = "M .1.2+0.4.2e0.4e0+8e-1.1.8 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
    { // Test relative
        char const *path_str = "m .1.2+0.3.0e0.0e0+6e-1-.3.0 z";
        Geom::PathVector pv = sp_svg_read_pathv(path_str);
        ASSERT_TRUE(bpathEqual(pv, pv_good)) << path_str;
    }
}

TEST_F(SvgPathGeomTest, testReadErrorMisplacedCharacter)
{

    char const *path_str;
    Geom::PathVector pv;
    // Comma in the wrong place (commas may only appear between parameters)
    path_str = "M 1,2 4,2 4,8 1,8 z , m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Comma in the wrong place (commas may only appear between parameters)
    path_str = "M 1,2 4,2 4,8 1,8 z m,13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Period in the wrong place (no numbers after a 'z')
    path_str = "M 1,2 4,2 4,8 1,8 z . m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Sign in the wrong place (no numbers after a 'z')
    path_str = "M 1,2 4,2 4,8 1,8 z + - m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Digit in the wrong place (no numbers after a 'z')
    path_str = "M 1,2 4,2 4,8 1,8 z 9809 m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Digit in the wrong place (no numbers after a 'z')
    path_str = "M 1,2 4,2 4,8 1,8 z 9809 876 m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
}
/*FAIL ??
TEST_F(SvgPathGeomTest, testReadErrorUnrecognizedCharacter)
{
    char const *path_str;
    Geom::PathVector pv;
    // Unrecognized character
    path_str = "M 1,2 4,2 4,8 1,8 z&m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Unrecognized character
    path_str = "M 1,2 4,2 4,8 1,8 z m &13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
}

TEST_F(SvgPathGeomTest, testReadErrorTypo)
{
    char const *path_str;
    Geom::PathVector pv;
    // Typo
    path_str = "M 1,2 4,2 4,8 1,8 z j 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;

    // Typo
    path_str = "M 1,2 4,2 4,8 1,8 L 1,2 x m 13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvopen)) << path_str;
}
*/
TEST_F(SvgPathGeomTest, testReadErrorIllformedNumbers)
{
    char const *path_str;
    Geom::PathVector pv;
    // Double exponent
    path_str = "M 1,2 4,2 4,8 1,8 z m 13e4e5,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Double sign
    path_str = "M 1,2 4,2 4,8 1,8 z m +-13,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Double sign
    path_str = "M 1,2 4,2 4,8 1,8 z m 13e+-12,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // No digit
    path_str = "M 1,2 4,2 4,8 1,8 z m .e12,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // No digit
    path_str = "M 1,2 4,2 4,8 1,8 z m .,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // No digit
    path_str = "M 1,2 4,2 4,8 1,8 z m +,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // No digit
    path_str = "M 1,2 4,2 4,8 1,8 z m +.e+,15";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
}

TEST_F(SvgPathGeomTest, testReadErrorJunk)
{
    char const *path_str;
    Geom::PathVector pv;
    // Junk
    path_str = "M 1,2 4,2 4,8 1,8 z j 357 hkjh.,34e34 90ih6kj4 h5k6vlh4N.,6,45wikuyi3yere..3487 m 13,23";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
}
/* FAIL ???
TEST_F(SvgPathGeomTest, testReadErrorStopReading)
{
    char const *path_str;
    Geom::PathVector pv;
    // Unrecognized parameter
    path_str = "M 1,2 4,2 4,8 1,8 z m #$%,23,34";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Invalid parameter
    path_str = "M 1,2 4,2 4,8 1,8 z m #$%,23,34";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;
    // Illformed parameter
    path_str = "M 1,2 4,2 4,8 1,8 z m +-12,23,34";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvclosed)) << path_str;

    // "Third" parameter
    path_str = "M 1,2 4,2 4,8 1,8 1,2,3 M 12,23";
    pv = sp_svg_read_pathv(path_str);
    ASSERT_TRUE(bpathEqual(pv, rectanglepvopen)) << path_str;
}
*/

TEST_F(SvgPathGeomTest, testRoundTrip)
{
    // This is the easiest way to (also) test writing path data, as a path can be written in more than one way.
    Geom::PathVector pv;
    Geom::PathVector new_pv;
    std::string org_path_str;
    std::string path_str;
    // Rectangle (closed)
    org_path_str = rectanglesAbsoluteClosed[0];
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv);
    new_pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, new_pv)) << org_path_str.c_str();
    // Rectangle (open)
    org_path_str = rectanglesAbsoluteOpen[0];
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv);
    new_pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, new_pv)) << org_path_str.c_str();
    // Concatenated rectangles
    org_path_str = rectanglesAbsoluteClosed[0] + rectanglesRelativeOpen[0] + rectanglesRelativeClosed[0] +
                   rectanglesAbsoluteOpen[0];
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv);
    new_pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, new_pv)) << org_path_str.c_str();
    // Zero-length subpaths
    org_path_str = "M 0,0 M 1,1 L 2,2 M 3,3 z M 4,4 L 5,5 z M 6,6";
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv);
    new_pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, new_pv)) << org_path_str.c_str();
    // Floating-point
    org_path_str = "M .01,.02 L 0.04,0.02 L.04,.08L0.01,0.08 z"
                   "M 1e-2,.2e-1 L 0.004e1,0.0002e+2 L04E-2,.08e0L1.0e-2,80e-3 z";
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv);
    new_pv = sp_svg_read_pathv(path_str.c_str());
    ASSERT_TRUE(bpathEqual(pv, new_pv, 1e-17)) << org_path_str.c_str();
    // Normalize path data
    org_path_str = "m 2,3 l 20,0.0003 h 10 v 10 q 50,10 40,25 t 70,25 l 0,10 c 5,6 10,5 10,10 s 10,20 5,6 z";
    pv = sp_svg_read_pathv(org_path_str.c_str());
    path_str = sp_svg_write_path(pv, true);
    ASSERT_TRUE(path_str == "M 2,3 L 22,3 L 32,3 L 32,13 C 65.33,19.67 78.67,28 72,38 C 65.33,48 88.67,56.33 142,63 L 142,73 C 147,79 152,78 152,83 C 152,88 162,103 157,89 Z");
}

TEST(PathVectorToBeziersTest, random)
{
    // Evil test will crash if not protected
    Geom::PathVector pathv = sp_svg_read_pathv("M349 683 A170 170 0 1 0 349.00000000000006 683");
    pathv_to_linear_and_cubic_beziers(pathv);
}

/*
 * Please do not change my prefs or put them back after :(
 * also, fails.

TEST_F(SvgPathGeomTest, testMinexpPrecision)
{
    Geom::PathVector pv;
    char *path_str;
    // Default values
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    prefs->setBool("/options/svgoutput/allowrelativecoordinates", true);
    prefs->setBool("/options/svgoutput/forcerepeatcommands", false);
    prefs->setInt("/options/svgoutput/numericprecision", 8);
    prefs->setInt("/options/svgoutput/minimumexponent", -8);
    pv = sp_svg_read_pathv("M 123456781,1.23456781e-8 L 123456782,1.23456782e-8 L 123456785,1.23456785e-8 L
10123456400,1.23456785e-8 L 123456789,1.23456789e-8 L 123456789,101.234564e-8 L 123456789,1.23456789e-8"); path_str =
sp_svg_write_path(pv); ASSERT_FALSE(  strcmp("m 123456780,1.2345678e-8 0,0 10,1e-15 9999999210,0 -9999999210,0
0,9.99999921e-7 0,-9.99999921e-7" , path_str )); g_free(path_str);
}*/

// vim: filetype=cpp:expandtab:shiftwidth=4:softtabstop=4:fileencoding=utf-8:textwidth=99 :
