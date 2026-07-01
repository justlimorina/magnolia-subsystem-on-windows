#include "Evaluator.h"
#include <sstream>
#include <stack>
#include <vector>
#include <cctype>
#include <cmath>
#include <map>

using Token = std::string;

static int precedence(const Token& op) {
	if (op == "+" || op == "-") return 1;
	if (op == "*" || op == "/") return 2;
	if (op == "^") return 3;
	return 0;
}

static bool isLeftAssociative(const Token& op) {
	if (op == "^") return false; // exponent is right-associative
	return true;
}

static bool isNumberChar(char c) {
	return (std::isdigit(static_cast<unsigned char>(c)) || c == '.');
}

static std::vector<Token> tokenize(const std::string& s, std::string& err) {
	std::vector<Token> tokens;
	size_t i = 0;
	while (i < s.size()) {
		if (std::isspace(static_cast<unsigned char>(s[i]))) { ++i; continue; }
		char c = s[i];
		if (isNumberChar(c)) {
			size_t j = i;
			while (j < s.size() && (isNumberChar(s[j]) || s[j] == 'e' || s[j] == 'E' || s[j] == '+' || s[j] == '-')) {
				// naive number parsing: include exponent part; we'll rely on stod later
				if ((s[j] == '+' || s[j] == '-') && j != i) {
					// allow signs in exponent only if previous char was 'e' or 'E'
					if (!(j>0 && (s[j-1]=='e' || s[j-1]=='E'))) break;
				}
				++j;
			}
			tokens.push_back(s.substr(i, j - i));
			i = j;
			continue;
		}
		if (std::isalpha(static_cast<unsigned char>(c))) {
			size_t j = i;
			while (j < s.size() && std::isalpha(static_cast<unsigned char>(s[j]))) ++j;
			tokens.push_back(s.substr(i, j - i));
			i = j;
			continue;
		}
		// operators and parentheses
		if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')') {
			tokens.push_back(std::string(1, c));
			++i;
			continue;
		}
		err = std::string("Unexpected character: ") + c;
		return {};
	}
	return tokens;
}

static bool isFunction(const Token& t) {
	static const std::map<std::string, int> funcs = {
		{"sin",1},{"cos",1},{"tan",1},{"log",1},{"ln",1},{"exp",1},{"sqrt",1}
	};
	return funcs.find(t) != funcs.end();
}

bool isSupportedFunction(const std::string& name) {
	return isFunction(name);
}

// Convert to RPN using shunting-yard
static bool toRPN(const std::vector<Token>& tokens, std::vector<Token>& out, std::string& err) {
	std::stack<Token> ops;
	for (size_t i = 0; i < tokens.size(); ++i) {
		const Token& t = tokens[i];
		// number
		if (!t.empty() && (std::isdigit(static_cast<unsigned char>(t[0])) || (t[0]=='.' && t.size()>1))) {
			out.push_back(t);
			continue;
		}
		if (isFunction(t)) {
			ops.push(t);
			continue;
		}
		if (t == ",") {
			// function argument separator not supported currently
			err = "Comma not supported";
			return false;
		}
		if (t == "+" || t == "-" || t == "*" || t == "/" || t == "^") {
			// handle unary minus: if at start or after '('
			if (t == "-") {
				bool unary = false;
				if (i == 0) unary = true;
				else {
					const Token& prev = tokens[i-1];
					if (prev == "(" || prev == "+" || prev == "-" || prev == "*" || prev == "/" || prev == "^") unary = true;
				}
				if (unary) {
					// represent unary minus as 'u-' token
					Token u = "u-";
					ops.push(u);
					continue;
				}
			}

			while (!ops.empty()) {
				Token top = ops.top();
				if ((top == "+" || top == "-" || top == "*" || top == "/" || top == "^") || top == "u-") {
					int p1 = precedence(top == "u-" ? "^" : top);
					int p2 = precedence(t);
					if ((isLeftAssociative(t) && p2 <= p1) || (!isLeftAssociative(t) && p2 < p1)) {
						out.push_back(top);
						ops.pop();
						continue;
					}
				}
				break;
			}
			ops.push(t);
			continue;
		}
		if (t == "(") {
			ops.push(t);
			continue;
		}
		if (t == ")") {
			bool foundLeft = false;
			while (!ops.empty()) {
				Token top = ops.top();
				ops.pop();
				if (top == "(") { foundLeft = true; break; }
				out.push_back(top);
			}
			if (!foundLeft) { err = "Mismatched parentheses"; return false; }
			// if function on top, pop it
			if (!ops.empty() && isFunction(ops.top())) { out.push_back(ops.top()); ops.pop(); }
			continue;
		}
		// identifier: maybe constant
		if (std::isalpha(static_cast<unsigned char>(t[0]))) {
			out.push_back(t);
			continue;
		}
		err = std::string("Unknown token: ") + t;
		return false;
	}
	while (!ops.empty()) {
		Token top = ops.top(); ops.pop();
		if (top == "(" || top == ")") { err = "Mismatched parentheses"; return false; }
		out.push_back(top);
	}
	return true;
}

static bool applyFunction(const std::string& fn, double arg, double& out) {
	if (fn == "sin") { out = std::sin(arg); return true; }
	if (fn == "cos") { out = std::cos(arg); return true; }
	if (fn == "tan") { out = std::tan(arg); return true; }
	if (fn == "log" || fn == "ln") { if (arg <= 0) return false; out = std::log(arg); return true; }
	if (fn == "exp") { out = std::exp(arg); return true; }
	if (fn == "sqrt") { if (arg < 0) return false; out = std::sqrt(arg); return true; }
	return false;
}

bool evaluateExpression(const std::string& expr, double& out, std::string& err) {
	std::string trimmed = expr;
	// trim
	size_t start = trimmed.find_first_not_of(" \t\n\r");
	if (start == std::string::npos) { err = "Empty expression"; return false; }
	trimmed = trimmed.substr(start);
	std::vector<Token> tokens = tokenize(trimmed, err);
	if (!err.empty()) return false;

	std::vector<Token> rpn;
	if (!toRPN(tokens, rpn, err)) return false;

	std::stack<double> st;
	for (const Token& t : rpn) {
		if (!t.empty() && (std::isdigit(static_cast<unsigned char>(t[0])) || t[0]=='.')) {
			try {
				double v = std::stod(t);
				st.push(v);
			} catch (...) { err = "Invalid number: " + t; return false; }
			continue;
		}
		if (t == "u-") {
			if (st.empty()) { err = "Missing operand for unary -"; return false; }
			double v = st.top(); st.pop(); st.push(-v); continue;
		}
		if (t == "+" || t == "-" || t == "*" || t == "/" || t == "^") {
			if (st.size() < 2) { err = "Missing operands for operator " + t; return false; }
			double b = st.top(); st.pop(); double a = st.top(); st.pop(); double r = 0;
			if (t == "+") r = a + b;
			else if (t == "-") r = a - b;
			else if (t == "*") r = a * b;
			else if (t == "/") { if (b == 0) { err = "Division by zero"; return false; } r = a / b; }
			else if (t == "^") r = std::pow(a, b);
			st.push(r);
			continue;
		}
		// function or constant
		if (std::isalpha(static_cast<unsigned char>(t[0]))) {
			// constants
			if (t == "pi") { st.push(3.14159265358979323846); continue; }
			if (t == "e") { st.push(2.71828182845904523536); continue; }
			// function: pop one arg
			if (isFunction(t)) {
				if (st.empty()) { err = "Missing argument for function " + t; return false; }
				double a = st.top(); st.pop(); double r;
				if (!applyFunction(t, a, r)) { err = "Invalid argument for function " + t; return false; }
				st.push(r);
				continue;
			}
			err = "Unknown identifier: " + t;
			return false;
		}
		err = "Unexpected token in RPN: " + t; return false;
	}
	if (st.size() != 1) { err = "Malformed expression"; return false; }
	out = st.top(); return true;
}
