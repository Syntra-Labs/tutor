use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub enum Type {
    Bool,
    Nat,
    Int,
    Int64,
    String,
    Named(String),
    List(Box<Type>),
    Unit,
}

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub enum Expr {
    Bool(bool),
    Nat(u64),
    Int(i64),
    String(String),
    Var(String),
    Call(String, Vec<Expr>),
    App(String, Vec<Expr>),
    Eq(Box<Expr>, Box<Expr>),
    Lt(Box<Expr>, Box<Expr>),
    Le(Box<Expr>, Box<Expr>),
    Gt(Box<Expr>, Box<Expr>),
    Add(Box<Expr>, Box<Expr>),
    Sub(Box<Expr>, Box<Expr>),
    Mul(Box<Expr>, Box<Expr>),
    Div(Box<Expr>, Box<Expr>),
    If(Box<Expr>, Box<Expr>, Box<Expr>),
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CtorDecl {
    pub name: String,
    pub fields: Vec<(String, Type)>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TypeDecl {
    pub name: String,
    pub ctors: Vec<CtorDecl>,
    pub unsupported: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Definition {
    pub name: String,
    pub params: Vec<(String, Type)>,
    pub ret: Type,
    pub body: Expr,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Module {
    pub name: String,
    pub types: Vec<TypeDecl>,
    pub definitions: Vec<Definition>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CBindings {
    pub header: String,
    pub rust: String,
}

pub fn generate_module(m: &Module) -> Result<String, String> {
    Ok(format!("// Module: {}\n", m.name))
}

pub fn generate_c_bindings(m: &Module) -> Result<CBindings, String> {
    let mut header = format!("/* C Bindings for {} */\n#pragma once\n#include <stdint.h>\n#include <stdbool.h>\n", m.name);
    for def in &m.definitions {
        header.push_str(&format!("uint8_t {}(void);\n", def.name));
    }
    Ok(CBindings {
        header,
        rust: String::new(),
    })
}
