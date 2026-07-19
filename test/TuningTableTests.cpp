#include "TuningTable.h"
#include <gtest/gtest.h>

TEST (TuningTableTest, StandardTuningHasZeroShiftForEveryString)
{
    for (int stringIndex = 0; stringIndex < 6; ++stringIndex)
        EXPECT_EQ (semitoneShiftForString (0, stringIndex), 0);
}

TEST (TuningTableTest, DropDLowersOnlyTheLowString)
{
    int const dropDIndex = 1;
    EXPECT_EQ (semitoneShiftForString (dropDIndex, 0), -2);
    for (int stringIndex = 1; stringIndex < 6; ++stringIndex)
        EXPECT_EQ (semitoneShiftForString (dropDIndex, stringIndex), 0);
}

TEST (TuningTableTest, GABDEGTuningMatchesHandComputedShifts)
{
    int const tuningIndex = 2; // "G A B D E G"
    int const expected[6] = { 3, 0, -3, -5, 5, 3 };
    for (int stringIndex = 0; stringIndex < 6; ++stringIndex)
        EXPECT_EQ (semitoneShiftForString (tuningIndex, stringIndex), expected[stringIndex]);
}

TEST (TuningTableTest, EveryShiftIsWithinAShortestPathOfSixSemitones)
{
    for (size_t tuningIndex = 0; tuningIndex < kTuningPresets.size(); ++tuningIndex)
        for (int stringIndex = 0; stringIndex < 6; ++stringIndex)
        {
            int const shift = semitoneShiftForString (static_cast<int> (tuningIndex), stringIndex);
            EXPECT_GE (shift, -6);
            EXPECT_LE (shift, 6);
        }
}

TEST (TuningTableTest, ChoiceListsHaveOneEntryPerPresetAndString)
{
    EXPECT_EQ (tuningChoices().size(), static_cast<int> (kTuningPresets.size()));
    EXPECT_EQ (stringChoices().size(), 6);
}
