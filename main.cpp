#include <bits/stdc++.h>

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool isFunction(const std::string& s) {
    return s == "sin" || s == "cos" || s == "tg" || s == "ctg" || s == "exp";
}

int getPriority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

bool checkExpression(const std::string& expr) {
    std::stack<char> brackets;

    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];

        if (std::isspace(c)) continue;

        if (c == '(') {
            brackets.push(c);
        }
        else if (c == ')') {
            if (brackets.empty()) return false;
            brackets.pop();
        }
    }

    return brackets.empty();
}

std::queue<std::string> toRPN(const std::string& expr, double x) {
    std::stack<std::string> opStack;
    std::queue<std::string> output;
    std::string number;
    std::string funcName;

    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];

        if (std::isspace(c)) continue;

        if (std::isdigit(c) || c == '.') {
            number += c;
            if (i == expr.length() - 1 || !(std::isdigit(expr[i + 1]) || expr[i + 1] == '.')) {
                output.push(number);
                number.clear();
            }
        }
        else if (c == 'x') {
            output.push(std::to_string(x));
        }
        else if (std::isalpha(c)) {
            funcName.clear();
            while (i < expr.length() && std::isalpha(expr[i])) {
                funcName += expr[i];
                i++;
            }
            i--;

            if (isFunction(funcName)) {
                opStack.push(funcName);
            }
        }
        else if (c == '(') {
            opStack.push("(");
        }
        else if (c == ')') {
            while (!opStack.empty() && opStack.top() != "(") {
                output.push(opStack.top());
                opStack.pop();
            }
            if (!opStack.empty()) opStack.pop();

            if (!opStack.empty() && isFunction(opStack.top())) {
                output.push(opStack.top());
                opStack.pop();
            }
        }
        else if (isOperator(c)) {
            std::string opStr(1, c);

            if (c == '-' && (i == 0 || expr[i - 1] == '(' || isOperator(expr[i - 1]))) {
                opStack.push("~");
            }
            else {
                while (!opStack.empty() && opStack.top() != "(" &&
                       ((opStack.top() == "~") ||
                        (isOperator(opStack.top()[0]) && getPriority(opStack.top()[0]) >= getPriority(c)))) {
                    output.push(opStack.top());
                    opStack.pop();
                }
                opStack.push(opStr);
            }
        }
    }

    while (!opStack.empty()) {
        output.push(opStack.top());
        opStack.pop();
    }

    return output;
}

double calculateRPN(std::queue<std::string> rpn) {
    std::stack<double> calcStack;

    while (!rpn.empty()) {
        std::string token = rpn.front();
        rpn.pop();

        if (isFunction(token)) {
            if (calcStack.empty()) {
                std::cout << "Error: Not enough operands for function " << token << std::endl;
                return 0;
            }

            double val = calcStack.top();
            calcStack.pop();

            if (token == "sin") val = std::sin(val);
            else if (token == "cos") val = std::cos(val);
            else if (token == "tg") val = std::tan(val);
            else if (token == "ctg") {
                double tanVal = std::tan(val);
                if (tanVal == 0) {
                    std::cout << "Error: ctg argument causes division by zero" << std::endl;
                    return 0;
                }
                val = 1.0 / tanVal;
            }
            else if (token == "exp") val = std::exp(val);

            calcStack.push(val);
        }
        else if (token == "~") {
            if (calcStack.empty()) {
                std::cout << "Error: Not enough operands for unary minus" << std::endl;
                return 0;
            }

            double val = calcStack.top();
            calcStack.pop();
            calcStack.push(-val);
        }
        else if (isOperator(token[0])) {
            if (calcStack.size() < 2) {
                std::cout << "Error: Not enough operands for operator " << token << std::endl;
                return 0;
            }

            double b = calcStack.top();
            calcStack.pop();
            double a = calcStack.top();
            calcStack.pop();

            switch (token[0]) {
                case '+': calcStack.push(a + b); break;
                case '-': calcStack.push(a - b); break;
                case '*': calcStack.push(a * b); break;
                case '/':
                    if (b == 0) {
                        std::cout << "Error: Division by zero." << std::endl;
                        return 0;
                    }
                    calcStack.push(a / b);
                    break;
            }
        }
        else {
            calcStack.push(std::stod(token));
        }
    }

    if (calcStack.empty()) {
        std::cout << "Error: No result" << std::endl;
        return 0;
    }

    return calcStack.top();
}

int main() {
    std::string expression;
    std::cout << "Enter expression: ";
    std::getline(std::cin, expression);

    if (!checkExpression(expression)) {
        std::cout << "Error: Invalid expression (brackets mismatch)." << std::endl;
        return 1;
    }

    double x = 0;
    bool hasX = expression.find('x') != std::string::npos;
    if (hasX) {
        std::cout << "Enter x = ";
        std::cin >> x;
    }

    std::queue<std::string> rpn = toRPN(expression, x);
    double result = calculateRPN(rpn);

    std::cout << "Result: " << result << std::endl;

    return 0;
}