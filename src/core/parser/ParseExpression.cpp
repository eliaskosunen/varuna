// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

// clang-format off
#include "core/parser/Parser.h"
// clang-format on
#include "ast/LiteralExpr.h"
#include "ast/OperatorExpr.h"

namespace core
{
namespace parser
{
    using namespace lexer;
    using namespace ast;

    std::unique_ptr<Expr> Parser::parsePrimary(bool tolerateUnrecognized)
    {
        // Parse primary expression
        switch(it->type.get())
        {
        case TOKEN_IDENTIFIER:
            return parseIdentifierExpression();
        case TOKEN_LITERAL_INTEGER:
            return parseIntegerLiteralExpression();
        case TOKEN_LITERAL_FLOAT:
            return parseFloatLiteralExpression();
        case TOKEN_LITERAL_CHAR:
            return parseCharLiteralExpression();
        case TOKEN_LITERAL_STRING:
            return parseStringLiteralExpression();
        case TOKEN_LITERAL_FALSE:
            return parseFalseLiteralExpression();
        case TOKEN_LITERAL_TRUE:
            return parseTrueLiteralExpression();
        case TOKEN_KEYWORD_LET:
            return parseVariableDefinition();
        default:
            if(tolerateUnrecognized)
            {
                return nullptr;
            }
            return parserError("Unknown token: '{}'", it->value);
        }
    }

    std::unique_ptr<Expr> Parser::parseExpression()
    {
        struct Operator
        {
            util::OperatorType type;
            util::SourceLocation loc;
        };

        std::stack<Operator> operators;
        std::stack<std::unique_ptr<Expr>> operands;
        int parenCount = 0;
        const auto beginExprIt = it;

        auto createOperator = [](lexer::Token t) {
            assert(t.type.get() >= 400);
            assert(t.type.get() < 700);
            return Operator{t.type.convert<util::OperatorType>(), t.loc};
        };

        while(it != endTokens)
        {
            if(it->type == TOKEN_PUNCT_PAREN_OPEN)
            {
                operators.push(createOperator(*it));
                ++it; // Skip '('
                ++parenCount;
                continue;
            }
            if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
            {
                if(parenCount == 0)
                {
                    break;
                }

                ++it; // Skip ')'
                --parenCount;
                if(parenCount >= 0)
                {
                    bool ok = false;
                    while(!operators.empty())
                    {
                        if(operators.top().type == util::PUNCT_PAREN_OPEN)
                        {
                            operators.pop();
                            ok = true;
                            break;
                        }

                        auto rhs = std::move(operands.top());
                        operands.pop();
                        auto lhs = std::move(operands.top());
                        operands.pop();

                        if(isAssignmentOperator(operators.top().type))
                        {
                            auto expr = createNode<AssignmentExpr>(
                                operators.top().loc, std::move(lhs),
                                std::move(rhs), operators.top().type);
                            operands.push(std::move(expr));
                        }
                        else
                        {
                            auto expr = createNode<BinaryExpr>(
                                operators.top().loc, std::move(lhs),
                                std::move(rhs), operators.top().type);
                            operands.push(std::move(expr));
                        }

                        operators.pop();
                    }
                    if(ok)
                    {
                        continue;
                    }
                }

                return parserError("Parenthesis mismatch in expression");
            }

            if(isBinaryOperator())
            {
                if(it == beginExprIt ||
                   getBinOpPrecedence(
                       (it - 1)->type.convert<util::OperatorType>()) >= 0)
                {
                    if(it->type == TOKEN_OPERATORB_ADD)
                    {
                        operators.push(createOperator(
                            Token(it->loc, TOKEN_OPERATORU_PLUS, it->value)));
                        ++it;
                        continue;
                    }
                    if(it->type == TOKEN_OPERATORB_SUB)
                    {
                        operators.push(createOperator(
                            Token(it->loc, TOKEN_OPERATORU_MINUS, it->value)));
                        ++it;
                        continue;
                    }
                }

                if(operators.empty() ||
                   operators.top().type == util::PUNCT_PAREN_OPEN ||
                   (getBinOpPrecedence() >
                    getBinOpPrecedence(operators.top().type)) ||
                   (isBinOpRightAssociative() &&
                    getBinOpPrecedence() ==
                        getBinOpPrecedence(operators.top().type)))
                {
                    operators.push(createOperator(*it));
                    ++it; // Skip operator
                    continue;
                }
                if(getBinOpPrecedence() <
                       getBinOpPrecedence(operators.top().type) ||
                   (!isBinOpRightAssociative() &&
                    getBinOpPrecedence() ==
                        getBinOpPrecedence(operators.top().type)))
                {
                    while(getBinOpPrecedence() <
                              getBinOpPrecedence(operators.top().type) ||
                          (!isBinOpRightAssociative() &&
                           getBinOpPrecedence() ==
                               getBinOpPrecedence(operators.top().type)))
                    {
                        auto rhs = std::move(operands.top());
                        operands.pop();
                        auto lhs = std::move(operands.top());
                        operands.pop();

                        if(isAssignmentOperator(operators.top().type))
                        {
                            auto expr = createNode<AssignmentExpr>(
                                operators.top().loc, std::move(lhs),
                                std::move(rhs), operators.top().type);
                            operands.push(std::move(expr));
                        }
                        else
                        {
                            auto expr = createNode<BinaryExpr>(
                                operators.top().loc, std::move(lhs),
                                std::move(rhs), operators.top().type);
                            operands.push(std::move(expr));
                        }

                        operators.pop();
                        if(operators.empty())
                        {
                            break;
                        }
                    }
                    operators.push(createOperator(*it));
                    ++it;
                }
                else
                {
                    return parserError("No sufficent operator rule found");
                }
            }
            else if(isPrefixUnaryOperator())
            {
                operators.push(createOperator(*it));
                ++it; // Skip operator
                continue;
            }
            else if(it->type == TOKEN_IDENTIFIER)
            {
                auto expr = parseIdentifierExpression();
                if(!expr)
                {
                    return nullptr;
                }
                operands.push(std::move(expr));
                continue;
            }
            else if(it->type == TOKEN_PUNCT_SEMICOLON ||
                    it->type == TOKEN_PUNCT_BRACE_OPEN ||
                    it->type == TOKEN_PUNCT_SQR_OPEN ||
                    it->type == TOKEN_PUNCT_COMMA)
            {
                break;
            }
            else if(it->type == TOKEN_EOF)
            {
                return parserError("Unexpected EOF in expression");
            }
            else
            {
                auto primary = parsePrimary(true);
                if(!primary)
                {
                    if(operators.empty())
                    {
                        break;
                    }
                    return parserError("Invalid token in expression: {} ({})",
                                       it->value, it->typeToString());
                }
                operands.push(std::move(primary));
                continue;
            }
        }

        while(!operators.empty())
        {
            if(isUnaryOperator(operators.top().type))
            {
                if(operands.empty())
                {
                    return parserError("Unary operators require 1 operand");
                }
                auto rhs = std::move(operands.top());
                operands.pop();

                auto expr = createNode<UnaryExpr>(
                    operators.top().loc, std::move(rhs), operators.top().type);
                operands.push(std::move(expr));

                operators.pop();
            }
            else
            {
                if(operands.size() < 2)
                {
                    return parserError("Binary operators require 2 operands");
                }
                auto rhs = std::move(operands.top());
                operands.pop();
                auto lhs = std::move(operands.top());
                operands.pop();

                if(isAssignmentOperator(operators.top().type))
                {
                    auto expr = createNode<AssignmentExpr>(
                        operators.top().loc, std::move(lhs), std::move(rhs),
                        operators.top().type);
                    operands.push(std::move(expr));
                }
                else
                {
                    auto expr = createNode<BinaryExpr>(
                        operators.top().loc, std::move(lhs), std::move(rhs),
                        operators.top().type);
                    operands.push(std::move(expr));
                }

                operators.pop();
            }
        }

        if(operands.empty())
        {
            parserWarning("Expression evaluated as empty");
            return createNode<EmptyExpr>(beginExprIt);
        }
        auto top = std::move(operands.top());
        return top;
    }

    std::unique_ptr<VariableDefinitionExpr> Parser::parseVariableDefinition()
    {
        // Variable definition syntax:
        // let [ qualifiers ] name [ : type ] [ = init-expression ] ;

        const auto iter = it;

        bool mut = false;
        ++it; // Skip 'let'

        if(it->type == TOKEN_KEYWORD_MUT)
        {
            mut = true;
            ++it; // Skip 'mut'
        }

        if(it->type != TOKEN_IDENTIFIER)
        {
            return parserError("Invalid variable definition: expected "
                               "identifier, got '{}' instead",
                               it->value);
        }
        std::string name = it->value;
        const auto nameIter = it;
        ++it; // Skip name

        std::string typen;
        const auto typeIter = it + 1;
        if(it->type == TOKEN_PUNCT_COLON)
        {
            ++it; // Skip ':'

            if(it->type != TOKEN_IDENTIFIER)
            {
                return parserError("Invalid variable definition: expected "
                                   "identifier after ':', got '{}' instead",
                                   it->value);
            }

            typen = it->value;
            ++it; // Skip type
        }

        // Parse possible init expression
        auto init = [&]() -> std::unique_ptr<Expr> {
            if(it->type == TOKEN_OPERATORA_SIMPLE)
            {
                ++it; // Skip '='
                return parseExpression();
            }

            // No init expression
            return createNode<EmptyExpr>(it);
        }();
        if(!init)
        {
            return nullptr;
        }

        auto name_ = createNode<IdentifierExpr>(nameIter, std::move(name));

        // Type will be inferred by the code generator
        if(typen.empty())
        {
            if(!init || init->nodeType == Node::EMPTY_EXPR)
            {
                return parserError("Invalid variable definition: init "
                                   "expression is required when type is not "
                                   "stated explicitly");
            }
            auto def = createNode<VariableDefinitionExpr>(
                iter, std::move(name_), std::move(init));
            def->isMutable = mut;
            return def;
        }

        auto typename_ = createNode<IdentifierExpr>(typeIter, std::move(typen));
        auto def = createNode<VariableDefinitionExpr>(
            iter, std::move(typename_), std::move(name_), std::move(init));
        def->isMutable = mut;
        return def;
    }

    std::unique_ptr<GlobalVariableDefinitionExpr>
    Parser::parseGlobalVariableDefinition()
    {
        const auto iter = it;
        auto var = parseVariableDefinition();
        if(!var)
        {
            return nullptr;
        }
        return createNode<GlobalVariableDefinitionExpr>(iter, std::move(var));
    }

    std::unique_ptr<Expr> Parser::parseIdentifierExpression()
    {
        std::string idName = it->value;
        const auto iter = it;
        ++it; // Skip identifier

        auto id = createNode<IdentifierExpr>(iter, std::move(idName));

        // Subscript
        if(it->type == TOKEN_PUNCT_SQR_OPEN)
        {
            return parseSubscriptExpression(std::move(id));
        }

        // Function call
        if(it->type == TOKEN_PUNCT_PAREN_OPEN)
        {
            return parseFunctionCallExpression(std::move(id));
        }

        return createNode<VariableRefExpr>(iter, std::move(id->value));
    }

    std::unique_ptr<ast::ArbitraryOperandExpr>
    Parser::parseFunctionCallExpression(std::unique_ptr<Expr> lhs)
    {
        const auto iter = it;
        ++it; // Skip '('
        std::vector<std::unique_ptr<Expr>> operands;
        operands.push_back(std::move(lhs));
        if(it->type != TOKEN_PUNCT_PAREN_CLOSE)
        {
            // Parse argument list
            while(it->type != TOKEN_EOF)
            {
                if(auto arg = parseExpression())
                {
                    operands.push_back(std::move(arg));
                }
                else
                {
                    return nullptr;
                }

                if(it->type == TOKEN_PUNCT_PAREN_CLOSE)
                {
                    break;
                }

                if(it->type != TOKEN_PUNCT_COMMA)
                {
                    return parserError("Invalid function call: Expected ')' or "
                                       "',' in argument list, got '{}' instead",
                                       it->value);
                }
                ++it; // Skip ','
            }
        }

        ++it; // Skip ')'
        return createNode<ArbitraryOperandExpr>(iter, std::move(operands),
                                                util::OPERATORC_CALL);
    }

    std::unique_ptr<BinaryExpr>
    Parser::parseSubscriptExpression(std::unique_ptr<Expr> lhs)
    {
        const auto iter = it;
        ++it; // Skip '['
        auto subscr = parseExpression();
        if(!subscr)
        {
            return nullptr;
        }

        if(it->type != TOKEN_PUNCT_SQR_CLOSE)
        {
            return parserError("Invalid subscript expression: Expected ']' "
                               "after expression, got '{}' instead",
                               it->value);
        }
        ++it; // Skip ']'

        return createNode<BinaryExpr>(iter, std::move(lhs), std::move(subscr),
                                      util::OPERATORB_SUBSCR);
    }

    std::unique_ptr<IntegerLiteralExpr> Parser::parseIntegerLiteralExpression()
    {
        const int base = [&]() {
            if(it->modifierInt.isSet(INTEGER_HEX))
            {
                return 16;
            }
            if(it->modifierInt.isSet(INTEGER_OCT))
            {
                return 8;
            }
            if(it->modifierInt.isSet(INTEGER_BIN))
            {
                return 2;
            }
            return 10;
        }();

        const auto lit = it;
        ++it;

        try
        {
            int64_t val = std::stoll(lit->value, nullptr, base);
            std::string type;
            bool isSigned = true;
            if(lit->modifierInt.isSet(INTEGER_INT8))
            {
                type = "i8";
                if(val > std::numeric_limits<int8_t>::max() ||
                   val < std::numeric_limits<int8_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int8", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT16))
            {
                type = "i16";
                if(val > std::numeric_limits<int16_t>::max() ||
                   val < std::numeric_limits<int16_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int16", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT32))
            {
                type = "i32";
                if(val > std::numeric_limits<int32_t>::max() ||
                   val < std::numeric_limits<int32_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into int32", lit->value));
                }
            }
            else if(lit->modifierInt.isSet(INTEGER_INT64))
            {
                type = "i64";
            }
            else if(lit->modifierInt.isSet(INTEGER_BYTE))
            {
                type = "byte";
                if(val > std::numeric_limits<uint8_t>::max() ||
                   val < std::numeric_limits<uint8_t>::min())
                {
                    throw std::out_of_range(
                        fmt::format("'{}' cannot fit into byte", lit->value));
                }
                isSigned = false;
            }
            else
            {
                throw std::invalid_argument(fmt::format(
                    "Invalid integer modifier: {}", lit->modifierInt.get()));
            }
            return createNode<IntegerLiteralExpr>(
                lit, val, createNode<IdentifierExpr>(lit, std::move(type)),
                isSigned);
        }
        catch(std::invalid_argument& e)
        {
            return parserError("Invalid integer literal: literal is "
                               "ill-formed: '{}'. Description: '{}'",
                               lit->value, e.what());
        }
        catch(std::out_of_range& e)
        {
            return parserError("Invalid integer literal: value out of range: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
    }
    std::unique_ptr<FloatLiteralExpr> Parser::parseFloatLiteralExpression()
    {
        const auto lit = it;
        ++it;

        try
        {
            double val;
            std::string type;
            std::tie(val, type) = [&]() -> std::tuple<double, std::string> {
                if(lit->modifierFloat.isSet(FLOAT_F64))
                {
                    return std::make_tuple<double, std::string>(
                        std::stod(lit->value), "f64");
                }
                if(lit->modifierFloat.isSet(FLOAT_F32))
                {
                    return std::make_tuple<double, std::string>(
                        static_cast<double>(std::stof(lit->value)), "f32");
                }
                throw std::invalid_argument(fmt::format(
                    "Invalid float modifier: {}", lit->modifierFloat.get()));
            }();
            return createNode<FloatLiteralExpr>(
                lit, val, createNode<IdentifierExpr>(lit, std::move(type)));
        }
        catch(std::invalid_argument& e)
        {
            return parserError("Invalid float literal: literal is ill-formed: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
        catch(std::out_of_range& e)
        {
            return parserError("Invalid float literal: value out of range: "
                               "'{}'. Description: '{}'",
                               lit->value, e.what());
        }
    }

    std::unique_ptr<StringLiteralExpr> Parser::parseStringLiteralExpression()
    {
        const auto lit = it;
        ++it;

        auto val = lit->value;
        if(!utf8::is_valid(val.begin(), val.end()))
        {
            return parserError("String literal contains invalid UTF-8: '{}'",
                               val);
        }
        auto type = createNode<IdentifierExpr>(lit, [&]() {
            if(lit->modifierString.isSet(STRING_C))
            {
                return "cstring";
            }
            return "string";
        }());
        return createNode<StringLiteralExpr>(lit, std::move(val),
                                             std::move(type));
    }

    std::unique_ptr<CharLiteralExpr> Parser::parseCharLiteralExpression()
    {
        const auto lit = it;
        ++it;

        if(lit->modifierChar.isSet(CHAR_BYTE))
        {
            if(lit->value.length() > 1)
            {
                return parserError("Invalid byte char literal: Value out of "
                                   "range: Value more than 1 byte: '{}'",
                                   lit->value);
            }
            auto t = createNode<IdentifierExpr>(lit, "bchar");
            return createNode<CharLiteralExpr>(lit, lit->value[0],
                                               std::move(t));
        }

        std::vector<char32_t> result;
        utf8::utf8to32(lit->value.begin(), lit->value.end(),
                       std::back_inserter(result));

        if(result.size() > 1)
        {
            return parserError("Invalid char literal: Value out of range: "
                               "Value more than one Unicode character (4 "
                               "bytes): '{}'",
                               lit->value);
        }
        assert(result.size() == 1);

        auto t = createNode<IdentifierExpr>(lit, "char");
        return createNode<CharLiteralExpr>(lit, result[0], std::move(t));
    }
    std::unique_ptr<BoolLiteralExpr> Parser::parseTrueLiteralExpression()
    {
        ++it;
        return createNode<BoolLiteralExpr>(it - 1, true);
    }
    std::unique_ptr<BoolLiteralExpr> Parser::parseFalseLiteralExpression()
    {
        ++it;
        return createNode<BoolLiteralExpr>(it - 1, false);
    }
} // namespace parser
} // namespace core
