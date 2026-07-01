#pragma once
#include <string>

// Evaluate a mathematical expression given as a string.
// Returns true on success and sets 'out'; returns false and sets 'err' on failure.
bool evaluateExpression(const std::string& expr, double& out, std::string& err);

// Optional: check if a token is a supported function/constant
bool isSupportedFunction(const std::string& name);
