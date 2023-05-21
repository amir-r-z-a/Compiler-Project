#include "Parser.h"

AST *Parser::parse() {
  AST *Res = parseLast();
  expect(Token::eoi);
  return Res;
}

AST *Parser::parseLast(){
Expr *E;
 
  if (Tok.is(Token::KW_type) || Tok.is(Token::ident))
  {
    E = parsePrg();
    advance();
    while (Tok.is(Token::semicolon))
    {
      advance();
      if (Tok.is(Token::KW_type) || Tok.is(Token::ident))
      {
        E = parsePrg();
      }
      advance();
    }
    if (consume(Token::semicolon))
      goto _error_type;
    
    if (expect(Token::eoi))
       goto _error;

    return E;
  
  }
  else
    goto _error_type;


_error_type:
  while (!Tok.is(Token::semicolon))
    advance();
  return nullptr;

_error:
  while (Tok.getKind() != Token::eoi)
    advance();
  return nullptr;

}

Expr *Parser::parseTypeInit() {
llvm::SmallVector<llvm::StringRef, 8> Vars;
  if (Tok.is(Token::KW_type))
  {
    advance();
    if (expect(Token::ident))
      goto _error_type;
    Vars.push_back(Tok.getText());
    advance();
    while (Tok.is(Token::comma))
    {
      advance();
      if (expect(Token::ident))
        goto _error_type;
      Vars.push_back(Tok.getText());
      advance();
    }
    if (consume(Token::semicolon))
      goto _error_type;
  }

_error_type:
  while (!Tok.is(Token::semicolon))
    advance();
  return nullptr;
}

Expr *Parser::parsePrg(){
  Expr *Res = nullptr;
    if (Tok.is(Token::KW_type)) {
      Res = parseTypeInit();

    }
    else if (Tok.is(Token::ident)) {
      Res = parseAssign();
    }
    
  return Res;

}

Expr *Parser::parseAssign()
{
    Expr *E;
    if (Tok.is(Token::ident))
    {
        advance();
        if (expect(Token::equal))
            goto _error1;
        advance();
        E = parseExpr();
        advance();

        if (consume(Token::semicolon))
            goto _error1;
    }

    return E;


_error1:
    while (!Tok.is(Token::semicolon))
        advance();
    return nullptr;
}

Expr *Parser::parseExpr() {
  Expr *Left = parseTerm();
  while (Tok.isOneOf(Token::plus, Token::minus)) {
    BinaryOp::Operator Op = Tok.is(Token::plus)
                                ? BinaryOp::Plus
                                : BinaryOp::Minus;
    advance();
    Expr *Right = parseTerm();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseTerm() {
  Expr *Left = parseFactor();
  while (Tok.isOneOf(Token::star, Token::slash)) {
    BinaryOp::Operator Op =
        Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
    advance();
    Expr *Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseFactor() {
  Expr *Res = nullptr;
  switch (Tok.getKind()) {
  case Token::number:
    Res = new Factor(Factor::Number, Tok.getText());
    advance(); break;
  case Token::ident:
    Res = new Factor(Factor::Ident, Tok.getText());
    advance(); break;
  case Token::l_paren:
    advance();
    Res = parseExpr();
    if (!consume(Token::r_paren)) break;
  default:
    if (!Res)
      error();
    while (!Tok.isOneOf(Token::r_paren, Token::star,
                        Token::plus, Token::minus,
                        Token::slash, Token::eoi))
      advance();
  }
  return Res;
}
