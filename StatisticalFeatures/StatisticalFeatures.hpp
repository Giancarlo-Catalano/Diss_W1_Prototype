//
// Created by gian on 19/09/22.
//

#ifndef DISS_SIMPLEPROTOTYPE_STATISTICALFEATURES_HPP
#define DISS_SIMPLEPROTOTYPE_STATISTICALFEATURES_HPP

#include "../Utilities/utilities.hpp"
#include "../Utilities/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

namespace GC {

    class StatisticalFeatures {
        using S = double;


    public:
        StatisticalFeatures() {
            //necessary for weird constructors
        }

        S average, standardDeviation;
#if USING_ALL_STATISTICAL_FEATURES == 1
        S firstQuantile, median, thirdQuantile;
        S minimum, maximum;
#endif

    public:

        template <class Container>
        static S getAverage(const Container& c) {
            ASSERT_NOT_EMPTY(c);
            S sum = 0;
            for (const auto& item : c)
                sum += item;
            return (S)(sum / c.size());
        }

        template <class Container>
        static S getStandardDeviation(const Container& c, const S precalculatedAverage) {
            ASSERT_NOT_EMPTY(c);
            auto squared = [](auto n) {return n*n;};
            auto distanceSquaredFromAverage = [&](const S x) {
                return squared(x - precalculatedAverage);
            };

            S sum = 0;
            for (const auto& x :c)
                sum += distanceSquaredFromAverage(x);

            return std::sqrt(sum / (c.size()-1)); //TODO this should be approximated better
        }


        template <class Container> //container of T's
        StatisticalFeatures(const Container& c){
            ASSERT_NOT_EMPTY(c);
            average = getAverage(c);
            standardDeviation = getStandardDeviation(c, average);

#if USING_ALL_STATISTICAL_FEATURES == 1
            Container sorted = c;
            std::sort(sorted.begin(), sorted.end());
            const size_t lastIndex = sorted.size()-1;
            const size_t middleIndex = lastIndex / 2;
            const size_t firstQuartileIndex = lastIndex/4;
            const size_t thirdQuartileIndex = (lastIndex*3)/4;
            minimum = sorted[0];
            firstQuantile = sorted[firstQuartileIndex];
            median = sorted[middleIndex];
            thirdQuantile = sorted[thirdQuartileIndex];
            maximum = sorted[lastIndex];
#endif
        }

        std::string to_string() const {
            Logger logger;
            logger.addVar("avg", average);
            logger.addVar("stdev", standardDeviation);
#if USING_ALL_STATISTICAL_FEATURES == 1
            logger.addVar("min", minimum);
            logger.addVar("firstQ", firstQuantile);
            logger.addVar("median", median);
            logger.addVar("thirdQ", thirdQuantile);
            logger.addVar("max", maximum);
#endif
            return logger.end();
        }
    };



} // GC

#endif //DISS_SIMPLEPROTOTYPE_STATISTICALFEATURES_HPP
