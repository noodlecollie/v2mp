#pragma once

#include <cstddef>

// Defining these here so that we can easily change
// whether line/column numbers are 0- or 1-based.
// This is mainly for column numbers, which I'm still
// not sure about.
static constexpr size_t LINE_NUMBER_BASE = 1;
static constexpr size_t COLUMN_NUMBER_BASE = 1;
