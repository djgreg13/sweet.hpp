#include <recdec.hpp>
#include <unit.hpp>
#include <format.hpp>
#include <logger.hpp>

ErrorStuff::ErrorStuff(const std::string& r, const std::string& p, const size_t d,
		const std::vector<std::string>& po) : rule(r), part(p), depth(d), possible(po)
{
} 

RecurDec::RecurDec(RuleStore& r, Output& o) : out(o), rs(r) {
}

void RecurDec::computeFirstSet() {
	bool changed = false;
	do {
		changed = false;
		for(auto& it : rs.rules) {
			std::string name = it.first;
			auto s = rs.first.find(name);
			if(s == rs.first.end()) {
				if(rs.rules.find(it.second.rule.front().name) == rs.rules.end()) {
					std::set<std::string> n;
					n.insert(it.second.rule.front().name);
					auto ret = rs.first.insert(std::make_pair(
						name,
						n
					));
	
					changed |= ret.second;
				} else {
					auto o = rs.first.find(it.second.rule.front().name);
					std::set<std::string> n;
					if(o != rs.first.end()) {
						for(auto& jt : o->second) {
							n.insert(jt);
						}
					}
					auto ret = rs.first.insert(std::make_pair(
						name,
						n
					));
					changed |= ret.second;
				}
			} else {
				if(rs.rules.find(it.second.rule.front().name) == rs.rules.end()) {
					s->second.insert(it.second.rule.front().name);
				} else {
					auto o = rs.first.find(it.second.rule.front().name);
					if(o != rs.first.end()) {
						for(auto& jt : o->second) {
							auto o2 = s->second.insert(jt);
							changed |= o2.second;
						}
					}
				}
			}
		}
	} while(changed);
}

std::vector<RulePart> getNameVector(const Expr& e) {
	std::vector<RulePart> ret;
	LOG();
	for(auto& it : e.rule) {
		std::cout<<it<<" ";
		ret.push_back(it);
	}
	std::cout<<std::endl;
	return ret;
}

void RecurDec::genRules() {
	//current = "VarDecl";
	//auto start = rs.rules.find(current);
	//ASSERT_T(start != rs.rules.end());
	//genRules(start->first);
	format(out.prsH, "// DO not MODIFY this FILE it IS generated\n\n");
	format(out.prsH, "#pragma once\n\n");
	format(out.prsH, "#include <ast.hpp>\n");
	format(out.prsH, "#include <token.hpp>\n");
	format(out.prsH, "#include <lexer.hpp>\n");
	format(out.prsS, "#include <lexer.hpp>\n");
	format(out.prsH, "\nclass Parser {\n");
	format(out.prsH, "public:\n");
	format(out.prsH, "\tParser(Lexer&);\n");

	format(out.prsS, "// DO not MODIFY this FILE it IS generated\n\n");
	format(out.prsS, "#include <parser.hpp>\n");
	format(out.prsS, "#include <ast.hpp>\n");
	format(out.prsS, "#include <error.hpp>\n");
	format(out.prsS, "\nParser::Parser(Lexer& l) : lexer(l) {\n");
	format(out.prsS, "\tthis->nextToken();\n");
	format(out.prsS, "}\n\n");
	format(out.prsS, "void Parser::nextToken() {\n");
	format(out.prsS, "\tthis->curToken = this->lexer.nextToken();\n");
	format(out.prsS, "}\n");

	std::set<std::string> done;
	/*for(auto& it : rs.token) {
		if(done.find(it.first) != done.end()) {
			continue;
		}

	}

	done.clear();*/
	for(auto& it : rs.rules) {
		if(done.find(it.first) != done.end()) {
			continue;
		}

		LOG("\n\n%s\n\n", it.first);
		current = it.first;
		genRules(it.first);
		done.insert(it.first);
	}
	format(out.prsH, "\nprivate:\n");
	format(out.prsH, "\tLexer& lexer;\n");
	format(out.prsH, "\tToken curToken;\n");
	format(out.prsH, "\tvoid nextToken();\n");
	format(out.prsH, "};\n");
}

void RecurDec::genAstForwardDecl() {
	format(out.astH, "// DO not MODIFY this FILE it IS generated\n\n");
	format(out.astH, "#pragma once\n\n");
	format(out.astH, "#include <cstdint>\n\n");
	format(out.astH, "#include <memory>\n");
	format(out.astH, "#include <ostream>\n");
	format(out.astH, "#include <string>\n");
	format(out.astH, "#include <token.hpp>\n\n");
	format(out.astH, "uint64_t getNextAstId();\n\n");
	format(out.astH, "class AstNode {\n"); 
	format(out.astH, "public:\n"); 
	format(out.astH, "\tAstNode();\n"); 
	format(out.astH, "\tuint64_t getId() const;\n"); 
	format(out.astH, "\tAstNode* getParent();\n"); 
	format(out.astH, "\tconst AstNode* getParent() const;\n"); 
	format(out.astH, "\tvoid setParent(AstNode*);\n"); 
	format(out.astH, "\tstd::ostream& operator<<(std::ostream&) const;\n"); 
	format(out.astH, "\tvirtual void toOutStream(std::ostream&,uint32_t=0) const = 0;\n"); 
	format(out.astH, "\tvirtual void toDotStream(std::ostream&) const = 0;\n"); 
	format(out.astH, "\nprivate:\n"); 
	format(out.astH, "\tuint64_t id;\n");
	format(out.astH, "\tAstNode* parent;\n");
	format(out.astH, "};\n\n"); 
	format(out.astH,  "\n// Forward Decl\n\n");
	std::set<std::string> done;
	for(auto& it : rs.rules) {
		if(done.find(it.first) != done.end()) {
			continue;
		}
		done.insert(it.first);
		format(out.astH, "class %s;\n", it.first);
		format(out.astH, "typedef std::shared_ptr<%s> %sPtr;\n", it.first, it.first);
		format(out.astH, "typedef std::shared_ptr<const %s> %sConstPtr;\n", it.first, it.first);
	}
	format(out.astH,  "\n\n// Decl");
	format(out.astH, "\n");

	format(out.astS, "// DO not MODIFY this FILE it IS generated\n\n");
	format(out.astS, "#include <ast.hpp>\n\n");
	format(out.astS, "uint64_t getNextAstId() {\n\tstatic uint64_t globalId;\n\treturn globalId++;\n}\n\n");
	format(out.astS, "AstNode::AstNode() : id(getNextAstId()), parent(nullptr) {\n}\n\n");
	format(out.astS, "uint64_t AstNode::getId() const {\n\treturn this->id;\n}\n\n");
	format(out.astS, "const AstNode* AstNode::getParent() const {\n\treturn this->parent;\n}\n\n");
	format(out.astS, "AstNode* AstNode::getParent() {\n\treturn this->parent;\n}\n\n");
	format(out.astS, "void AstNode::setParent(AstNode* p) {\n\tthis->parent = p;\n}\n\n");
	format(out.astS, "std::ostream& AstNode::operator<<(std::ostream& ss) const "
		"{\n\ttoOutStream(ss);\n\treturn ss;\n}\n\n"
	);
}

size_t RecurDec::newErrorStuff(const std::string& rule, const std::string& part, const size_t depth,
		const std::vector<std::string>& p) {
	errorStuff.emplace_back(rule, part, depth, p);
	return errorStuff.size() - 1u;
}

void RecurDec::writeErrorStuff() {
	format(out.errH, "#pragma once\n\n");
	format(out.errH, "#include <vector>\n\n");
	format(out.errH, "#include <token.hpp>\n");
	format(out.errH, "#include <ast.hpp>\n\n");
	format(out.errH, "class ParseException : public std::exception {\n");
	format(out.errH, "public:\n");
	format(out.errH, "\tconst size_t id;\n");
	format(out.errH, "\tconst Token token;\n\n");
	format(out.errH, "\tParseException(const Token&, const size_t);\n");
	format(out.errH, "\tconst char* what() const throw();\n");
	format(out.errH, "};\n\n");

	format(out.errH, "struct ErrorType {\n");
	format(out.errH, "\tconst size_t id;\n");
	format(out.errH, "\tconst std::string rule;\n");
	format(out.errH, "\tconst std::string part;\n");
	format(out.errH, "\tconst size_t depth;\n");
	format(out.errH, "\tconst std::vector<std::string> follow;\n");
	format(out.errH, "\tErrorType(const size_t,const std::string&,const std::string&,const size_t,"
		"std::initializer_list<std::string>);\n"
	);
	format(out.errH, "};\n\n");
	format(out.errH, "const ErrorType& getError(const size_t);\n\n");
	format(out.errH, "typedef std::vector<ErrorType> ErrorTypeVector;\n\n");
	format(out.errH, "const ErrorTypeVector vectorOfError = {\n");
	for(size_t i = 0; i < errorStuff.size(); ++i) {
		format(out.errH, "\tErrorType(%u, \"%s\", \"%s\", %u, {\n", i, errorStuff[i].rule, 
			errorStuff[i].part, errorStuff[i].depth
		);
		for(size_t j = 0; j < errorStuff[i].possible.size(); ++j) {
			format(out.errH, "\t\t\"%s\"%s", errorStuff[i].possible[j],
				j+1 == errorStuff[i].possible.size() ? "\n" : ",\n"
			);
		}
		format(out.errH, "\t})%s", i+1 == errorStuff.size() ? "\n" : ",\n");
	}
	format(out.errH, "};\n");

	format(out.errS, "#include <string>\n");
	format(out.errS, "#include <sstream>\n\n");
	format(out.errS, "#include <%s>\n\n", out.errHfn);
	format(out.errS, "ParseException::ParseException(const Token& t, const size_t i) : "
		"id(i), token(t) {}\n\n"
	);
	format(out.errS, "const char* ParseException::what() const throw() {\n");
	format(out.errS, "\tstd::stringstream ss;\n\tss<<this->token<<\" \"<<this->id;\n");
	format(out.errS, "\treturn ss.str().c_str();\n}\n\n");
	format(out.errS, "ErrorType::ErrorType(const size_t i,const std::string& r, const std::string& p, "
		"const size_t d, \n\tstd::initializer_list<std::string> f) : \n"
	);
	format(out.errS, "\tid(i), rule(r), part(p), depth(d), follow(f) {\n");
	format(out.errS, "}\n");
}

void RecurDec::walkTrie(const GrammarPrefix::TrieEntry* path, const std::string& part, const size_t depth) {
	std::string prefix(depth, '\t');
	bool wasFirst = false;
	if(path->map.empty()) {
		return;
	}
	for(auto& it : path->map) {
		bool pushed = false;
		if(rs.rules.find(it.first.name) != rs.rules.end()) {
			format(out.prsS, "%s%s(lookAheadTest%s(curToken)) {\n", 
				wasFirst ? "" : prefix, 
				wasFirst ? " else if" : "if", it.first.name
			);
		} else {
			format(out.prsS, "%s%s(curToken.type == TokenType::%s) {\n", 
				wasFirst ? "" : prefix, 
				wasFirst ? " else if" : "if", it.first.name
			);

		}
		if(rs.token.find(it.first.name) != rs.token.end() && 
				!it.first.storeName.empty()) {
			nameStack.push_back(it.first.storeName);
			pushed = true;
			format(out.prsS, "%s\tToken %s(curToken);\n",prefix, 
				it.first.storeName
			);
			format(out.prsS, "%s\tnextToken();\n", prefix);
		} else if(rs.rules.find(it.first.name) != rs.rules.end() && 
				!it.first.storeName.empty()) {
			nameStack.push_back(it.first.storeName);
			pushed = true;
			format(out.prsS, "%s\t%sPtr %s(parse%s());\n", prefix, it.first.name, 
				it.first.storeName, it.first.name
			);
		} else {
			format(out.prsS, "%s\tnextToken();\n", prefix);
		}
		walkTrie(&(it.second), it.first.name, depth+1);
		if(it.second.isValue) {
			format(out.prsS, "\t%sreturn std::make_shared<%s>(", prefix, 
				current
			);
			for(auto& name : nameStack) {
				format(out.prsS, "%s, ", name);
			}
			format(out.prsS, "\n%s\t\t%sEnum::%s\n", prefix, current, it.second.value);
			format(out.prsS, "\t%s);\n", prefix);
		}
		if(pushed) {
			nameStack.pop_back();
		}
		format(out.prsS, "%s}", prefix);
		wasFirst = true;
	}
	std::vector<std::string> possible;
	for(auto& it : path->map) {
		possible.push_back(it.first.name);
	}
		
	if(!path->isValue) {
		format(out.prsS, " else {\n");
		format(out.prsS, "%s\tthrow ParseException(curToken, %u", prefix,
				newErrorStuff(current, part, depth, possible)
		);
		format(out.prsS, ");\n");
		format(out.prsS, "%s}", prefix);
	}	
	format(out.prsS, "\n");
}

void RecurDec::genRules(const std::string& start) {
	allreadyDone.insert(start);

	// header
	const std::string headerStringTest("\tbool lookAheadTest%s(const Token&);\n");
	const std::string headerStringParse("\t%sPtr parse%s();\n");

	format(out.prsH, "\n\n\t// %s\n", current);
	format(out.prsH, headerStringTest, start);
	format(out.prsH, headerStringParse, start, start);


	// source
	const std::string srcStringTest("bool Parser::lookAheadTest%s(const Token& token) {\n");
	const std::string srcStringParse("%sPtr Parser::parse%s() {\n");

	format(out.prsS, "\n\n// %s\n\n", current);
	format(out.prsS, srcStringTest, start);
	format(out.prsS, "\treturn");
	auto lookAheadSet = rs.first.find(start);
	ASSERT_T(lookAheadSet != rs.first.end());
	const size_t lookAheadSetSize = lookAheadSet->second.size();
	size_t i = 0;
	for(auto& it : lookAheadSet->second) {
		format(out.prsS, "%stoken.type == TokenType::%s", (i != 0 ? "\t\t" : " "), it);
		if(i+1 == lookAheadSetSize) {
			format(out.prsS, ";\n");
		} else {
			format(out.prsS, " ||\n");
		}
		++i;
	}
	format(out.prsS, "}\n\n");

	GrammarPrefix trie;
	GrammarPrefixEnum trieEnum;
	auto range = rs.rules.equal_range(start);
	for(; range.first != range.second; ++range.first) {
		std::string endName = range.first->second.rule[0].endName;
		auto vec = getNameVector(range.first->second);
		trie.insert(vec.begin(), vec.end(), endName);
		trieEnum.insert(vec.begin(), vec.end(), endName);
	}

	format(out.prsS, srcStringParse, start, start);
	walkTrie(&trie.getRoot(), start, 1);
	LOG("The trie");
	std::cout<<trie<<std::endl;
	std::cout<<trieEnum<<std::endl;
	format(out.prsS, "}\n");

	this->store.clear();
	this->store2.clear();
	walkTrieConstructor(&trie.getRoot(), std::vector<RulePart>());
	walkTrieConstructor2(&trieEnum.getRoot(), std::vector<RulePart>());
	for(auto& it : this->store) {
		LOG();
		for(auto& jt : it) {
			format(std::cout, "%s ", jt);
		}
		std::cout<<std::endl;
	}
	this->genAst(this->store);

}

void RecurDec::walkTrieConstructor2(const GrammarPrefixEnum::TrieEntry* path, 
		std::vector<RulePart> cur) {
	if(path->isValue) {
		this->store2.push_back(cur);
	}

	for(auto& it : path->map) {
		std::vector<RulePart> n(cur.begin(), cur.end());
		if(!it.first.storeName.empty()) {
			n.push_back(it.first);
		}
		walkTrieConstructor2(&(it.second), n);
	}
}

void RecurDec::walkTrieConstructor(const GrammarPrefix::TrieEntry* path, 
		std::vector<RulePart> cur) {
	if(path->isValue) {
		this->store.push_back(cur);
	}

	for(auto& it : path->map) {
		std::vector<RulePart> n(cur.begin(), cur.end());
		if(!it.first.storeName.empty()) {
			n.push_back(it.first);
		}
		walkTrieConstructor(&(it.second), n);
	}
}

void RecurDec::genAstClassDeclStart() {
	format(out.astH, "class %s : public AstNode {\n", current);
	format(out.astH, "public:\n");
}

void RecurDec::genAstOutputMethods(const std::map<std::string,std::vector<RulePart>>& rules) {
	format(out.astS, "void %s::toOutStream(std::ostream& ss, uint32_t depth) const {\n", current);
	format(out.astS, "\tconst std::string indent(depth, ' ');\n");
	format(out.astS, "\tss<<indent<<\"%s\"<<\" id(\"<<", current);
	out.astS<<"this->getId()"<<"<<\") rule(\"<<this->rule<<\")\"<<std::endl;\n";
	bool first = true;
	for(auto& it : rules) {
		format(out.astS, "\t%s(this->rule == %sEnum::%s) {\n", first ? "if" : "} else if",
			current, it.first
		);
		first = false;
		LOG("%s", it.second);
		for(auto& jt : it.second) {
			if(!jt.storeName.empty()) {
				if(rs.token.find(jt.name) != rs.token.end()) {
					format(out.astS, "\t\tthis->%s.toOutStream(ss, depth+1);\n", jt.storeName);
				} else {
					format(out.astS, "\t\tthis->%s->toOutStream(ss, depth+1);\n", jt.storeName);
				}
			}
		}
	}
	if(!rules.empty()) {
		format(out.astS, "\t}\n");
	}
	format(out.astS, "}\n");
}

void RecurDec::genAstClassDeclEnd(const std::set<RulePart>& allValues) {
	std::set<std::string> allreadyProcessed;
	for(auto& it : allValues) {
		std::string name = it.storeName;
		name[0] = toupper(name[0]);
		if(allreadyProcessed.count(name)) {
			continue;
		}
		if(rs.token.find(it.name) != rs.token.end()) {
			format(out.astH, "\n\tToken& get%s();\n", name);
			format(out.astH, "\tconst Token& get%s() const;\n", name);

			format(out.astS, "\nToken& %s::get%s() {\n", current, name);
			format(out.astS, "\treturn this->%s;\n}\n\n", it.storeName);
			format(out.astS, "const Token& %s::get%s() const {\n", current, name);
			format(out.astS, "\treturn this->%s;\n}\n\n", it.storeName);
		} else {
			format(out.astH, "\n\t%sPtr get%s();\n", it.name, name);
			format(out.astH, "\t%sConstPtr get%s() const;\n", it.name, name);

			format(out.astS, "\n%sPtr %s::get%s() {\n", it.name, current, name);
			format(out.astS, "\treturn this->%s;\n}\n\n", it.storeName);
			format(out.astS, "%sConstPtr %s::get%s() const {\n", it.name, current, name);
			format(out.astS, "\treturn this->%s;\n}\n", it.storeName);
		}
		allreadyProcessed.insert(name);
	}
	format(out.astH, "\n\t%sEnum getRule() const;\n\n", current);
	format(out.astH, "\tvoid toOutStream(std::ostream&,uint32_t=0) const override;\n"); 
	format(out.astH, "\tvoid toDotStream(std::ostream&) const override;\n\n"); 
	format(out.astH, "private:\n");
	std::map<std::string,std::string> names;
	for(auto& it : allValues) {
		LOG("allValues | %s %s", it.storeName, it.name);
		names.insert(std::make_pair(it.storeName,it.name));
	}

	for(auto& it : names) {
		//if(rs.token.find(it.name) != rs.token.end()) {
		LOG(" it = %s", it.first);
		if(rs.token.find(it.second) != rs.token.end()) {
			//format(out.astH, "\tToken %s;\n", it.storeName);
			format(out.astH, "\tToken %s;\n", it.first);
		} else if(rs.rules.count(it.second)) {
			//format(out.astH, "\t%sPtr %s;\n", it.name, it.storeName);
			format(out.astH, "\t%sPtr %s;\n", it.second, it.first);
		} else {
			ASSERT_T_MSG(false, "nor token neither rule");
		}
	}
	format(out.astH, "\t%sEnum rule;\n", current);
	format(out.astH, "};\n");
}

std::ostream& operator<<(std::ostream& ss, const std::vector<RulePart>& r) {
	for(auto& it : r) {
		ss<<it<<" ";
	}
	return ss;
}

std::ostream& operator<<(std::ostream& ss, const std::vector<std::string>& r) {
	for(auto& it : r) {
		ss<<it<<" ";
	}
	return ss;
}

void RecurDec::genAst(const std::vector<std::vector<RulePart>>& r) {
	format(out.astH, "\n\n// %s\n\n", current);
	format(out.astS, "\n// %s\n\n", current);
	std::set<RulePart> allToStore;
	std::map<std::string,std::vector<RulePart>> enumNames;
	std::map<std::string,std::vector<RulePart>> enumNames2;
	std::set<std::string> moreEnumNames;
	auto range = rs.rules.equal_range(current);
	for(; range.first != range.second; range.first++) {
		for(auto jt : range.first->second.rule) {
			if(!jt.storeName.empty()) {
				allToStore.insert(jt);
			}
			moreEnumNames.insert(jt.endName);
		}
	}
	std::vector<std::vector<RulePart>> minimized;
	size_t cnt = 0;
	for(auto& it : r) {
		++cnt;
		bool equal = false;
		LOG("%s\n", it);
		for(auto& jt : minimized) {
			if(it.size() != jt.size()) {
				equal = false;
				break;
			} 
			for(size_t i = 0; i < it.size(); ++i) {
				if(rs.token.find(it[i].name) != rs.token.end() && 
						rs.token.find(jt[i].name) != rs.token.end()) {
					equal = true;
					continue;
				}
				if(it[i].name != jt[i].name) {
					equal = false;
					goto end;
				}
			}
		}
		end:
		LOG("equal = %b |", equal);
		if(it.empty()) {
			continue;
		}
		//for(auto& jt : it) {
			LOG("%s | %s %u", it[0].endName, it, it.size());
			enumNames.insert(std::make_pair(it[0].endName, it));
			//enumNames2.insert(std::make_pair(it[0].endName, cnt-1));
		//}
		std::cout<<std::endl;
		if(!equal) {
			LOG("INSERT %s", it);
			minimized.push_back(it);
		}
	}
	std::vector<std::vector<RulePart>> minimized2;
	cnt = 0;
	for(auto& it : this->store2) {
		++cnt;
		bool equal = false;
		LOG("%s\n", it);
		for(auto& jt : minimized2) {
			if(it.size() != jt.size()) {
				equal = false;
				break;
			} 
			for(size_t i = 0; i < it.size(); ++i) {
				if(rs.token.find(it[i].name) != rs.token.end() && 
						rs.token.find(jt[i].name) != rs.token.end()) {
					equal = true;
					continue;
				}
				if(it[i].name != jt[i].name) {
					equal = false;
					goto end2;
				}
			}
		}
		end2:
		LOG("equal = %b |", equal);
		if(it.empty()) {
			continue;
		}
		//for(auto& jt : it) {
		LOG("%s | %s %u", it[0].endName, it, it.size());
		//enumNames.insert(std::make_pair(it[0].endName, it));
		enumNames2.insert(std::make_pair(it[0].endName, it));
		//}
		/*std::cout<<std::endl;
		if(!equal) {
			LOG("INSERT %s", it);
			minimized.push_back(it);
		}*/
	}
	std::vector<std::vector<std::string>> dupCon;
	for(auto& it : minimized) {
		if(it.empty()) {
			continue;
		}
		//allToStore.insert(it.begin(), it.end());
		std::vector<std::string> myCon;
		for(auto& jt : it) {
			if(rs.token.find(jt.name) != rs.token.end()) {
				myCon.push_back(format("const Token&"));
			} else if(rs.rules.find(jt.name) != rs.rules.end()) {
				myCon.push_back(format("%sPtr", jt.name));
			}
		}
		bool allreadyThere = false;
		for(auto& jDup : dupCon) {
			if(jDup.size() != myCon.size()) {
				continue;
			}
			for(size_t i = 0; i < myCon.size(); ++i) {
				if(jDup[i] != myCon[i]) {
					goto cont;
				}
			}
			allreadyThere = true;
			cont: continue;
		}
		if(allreadyThere) {
			continue;
		}
		dupCon.push_back(myCon);
		format(out.astS, "%s::%s(", this->current, this->current);
		/*const size_t itSize = it.size();
		size_t idx = 0;*/
		for(auto& jt : it) {
			if(rs.token.find(jt.name) != rs.token.end()) {
				format(out.astS, "const Token& %s_arg%s", jt.storeName, ", ");
			} else if(rs.rules.find(jt.name) != rs.rules.end()) {
				format(out.astS, "%sPtr %s_arg%s", jt.name, jt.storeName, ", ");
			} else {
				//ASSERT_T_MSG(false, "Not reachable");
			}
			//++idx;
		}
		format(out.astS, "const %sEnum enumType)\n\t: ", current);

		std::vector<RulePart> sorted(it.begin(), it.end());
		std::sort(sorted.begin(), sorted.end(), [](const RulePart& a, const RulePart& b) {
			return a.storeName < b.storeName;
		});
		LOG("sorted NAMES | %s\n", sorted);
		for(auto& jt : sorted) {
			format(out.astS, "%s(%s_arg), ", jt.storeName, jt.storeName);
		}
		format(out.astS, "rule(enumType)\n{\n}\n\n");
	}

	format(out.astS, "%s::%s(", current, current);
	format(out.astS, "const %sEnum enumType)\n\t: ", current);
	format(out.astS, "rule(enumType)\n{\n}\n");

	format(out.astH, "enum class %sEnum {\n", current);
	std::set<std::string> setNames;
	for(auto& it : moreEnumNames) {
		setNames.insert(it);
	}
	const size_t enumNamesS = setNames.size();
	size_t idx = 0;
	for(auto& it : setNames) {
		format(out.astH, "\t%s%s", it, idx+1 == enumNamesS ? "\n" : ",\n");
		++idx;
	}
	format(out.astH, "};\n\n");

	format(out.astH, "std::ostream& operator<<(std::ostream&, const %sEnum);\n\n", current);
	format(out.astS, "\nstd::ostream& operator<<(std::ostream& ss, const %sEnum en) {\n", current);
	bool first = true;
	for(auto& it : setNames) {
		format(out.astS, "%s(en == %sEnum::%s) {\n\t\tss<<\"%s\";\n\t}", 
			first ? "\tif" : " else if", current, it, it
		);
		first = false;
	}
	format(out.astS, " else {\n\t\tthrow std::logic_error(std::string(\"No %sEnum for value \'\")"
		"\n\t\t\t+ std::to_string(static_cast<int>(en)) + \"'\"\n\t\t);\n\t}\n", current
	);
	format(out.astS, "\treturn ss;\n");
	format(out.astS, "}\n\n");

	format(out.astS, "void %s::toDotStream(std::ostream& ss) const {\n", current); 
	format(out.astS, "\tss<<\"node\"<<this->getId()<<\" [label=\\\"\";\n");
	format(out.astS, "\tss<<\"<table border=\\\"0\\\" cellborder=\\\"0\\\" cellpadding=\\\"3\\\" \";\n");
	format(out.astS, "\tss<<\"bgcolor=\\\"white\\\">\";\n");
	format(out.astS, "\tss<<\"<tr>\";\n");
	format(out.astS, "\tss<<\"\\t<td bgcolor=\\\"black\\\" align=\\\"center\\\" colspan=\\\"2\\\">\";\n");
	format(out.astS, "\tss<<\"\\t\\t<font color=\\\"white\\\">%s</font>\";\n", current);
	format(out.astS, "\tss<<\"\\t</td>\";\n");
	format(out.astS, "\tss<<\"</tr>\";\n");
	format(out.astS, "\tss<<\"<tr>\";\n");
	format(out.astS, "\tss<<\"\\t<td align=\\\"left\\\">Id</td>\";\n");
	format(out.astS, "\tss<<\"\\t<td align=\\\"right\\\">\"<<this->getId()<<\"</td>\";\n");
	format(out.astS, "\tss<<\"</tr>\";\n");
	format(out.astS, "\tss<<\"<tr>\";\n");
	format(out.astS, "\tss<<\"\\t<td align=\\\"left\\\">Rule</td>\";\n");
	format(out.astS, "\tss<<\"\\t<td align=\\\"right\\\">\"<<this->rule<<\"</td>\";\n");
	format(out.astS, "\tss<<\"</tr>\";\n");
	first = true;
	for(auto& it : enumNames2) {
		bool containsToken = false;
		for(auto& jt : it.second) {
			if(rs.token.count(jt.name)) {
				containsToken = true;
				goto br;
			}
		}
		br:
		if(!containsToken) {
			break;
		}
		format(out.astS, "%s(this->rule == %sEnum::%s) {\n", first ? "\tif" : " else if", 
			current, it.first
		);
		for(auto& jt : it.second) {
			if(rs.token.count(jt.name)) {
				format(out.astS, "\t\tss<<\"<tr>\";\n");
				format(out.astS, "\t\tss<<\"\\t<td align=\\\"left\\\">Token(%s)</td>\";\n", jt.storeName);
				format(out.astS, "\t\tss<<\"\\t<td align=\\\"right\\\">\"<<this->%s<<\"</td>\";\n",
					jt.storeName
				);
				format(out.astS, "\t\tss<<\"</tr>\";\n");
			}
		}
		format(out.astS, "\t}");
		first = false;

	}
	format(out.astS, "\n");
	format(out.astS, "\tss<<\"</table>\";\n");
	format(out.astS, "\tss<<\"\\\"]\";\n");
	first = true;
	for(auto& it : enumNames2) {
		LOG("%s", it.first);
		bool containsRules = false;
		for(auto& jt : it.second) {
			if(rs.rules.count(jt.name)) {
				containsRules = true;
				goto br4;
			}
		}
		br4:
		if(!containsRules) {
			break;
		}
		format(out.astS, "%s(this->rule == %sEnum::%s) {\n", first ? "\tif" : " else if", 
			current, it.first
		);
		LOG("%s %u", it.second, it.second.size());
		for(auto& jt : it.second) {
			if(rs.rules.count(jt.name)) {
				format(out.astS, "\t\tss<<this->%s;\n",
					jt.storeName
				);
			}
		}
		format(out.astS, "\t}");
		first = false;
	}
	format(out.astS, "\n\n");
	
	first = true;
	for(auto& it : enumNames2) {
		bool containsRules = false;
		for(auto& jt : it.second) {
			if(rs.rules.count(jt.name)) {
				containsRules = true;
				goto br2;
			}
		}
		br2:
		if(!containsRules) {
			break;
		}
		format(out.astS, "%s(this->rule == %sEnum::%s) {\n", first ? "\tif" : " else if", 
			current, it.first
		);
		for(auto& jt : it.second) {
			if(rs.rules.count(jt.name)) {
				format(out.astS, "\t\tss<<\"\\tnode\"<<this->getId()<<\" -> \"<<this->%s->getId()<<\";\";\n",
					jt.storeName
				);
			}
		}
		format(out.astS, "\t}");
		first = false;
	}
	
	format(out.astS, "}\n\n");

	this->genAstClassDeclStart();
	dupCon.clear();
	for(auto& it : minimized) {
		if(it.empty()) {
			continue;
		}
		std::vector<std::string> myCon;
		for(auto& jt : it) {
			if(rs.token.find(jt.name) != rs.token.end()) {
				myCon.push_back(format("const Token&"));
			} else if(rs.rules.find(jt.name) != rs.rules.end()) {
				myCon.push_back(format("%sPtr", jt.name));
			}
		}
		LOG("myCon = %s", myCon);
		bool allreadyThere = false;
		for(auto& jDup : dupCon) {
			LOG("jDup = %s", jDup);
			if(jDup.size() != myCon.size()) {
				continue;
			}
			for(size_t i = 0; i < myCon.size(); ++i) {
				if(jDup[i] != myCon[i]) {
					goto cont2;
				}
			}
			allreadyThere = true;
			cont2: continue;
		}
		if(allreadyThere) {
			continue;
		}
		LOG("new");
		dupCon.push_back(myCon);
		format(out.astH, "\t%s(", this->current);
		for(auto& jt : it) {
			if(rs.token.find(jt.name) != rs.token.end()) {
				format(out.astH, "const Token&%s", ", ");
			} else if(rs.rules.find(jt.name) != rs.rules.end()) {
				format(out.astH, "%sPtr%s", jt.name, ", ");
			} else {
				ASSERT_T_MSG(false, format("Not reachable '%s'",jt.name));
			}
			//++idx;
		}
		format(out.astH, "const %sEnum);\n", current);
	}
	format(out.astH, "\t%s(", this->current);
	format(out.astH, "const %sEnum);\n", current);
	this->genAstOutputMethods(enumNames2);
	this->genAstClassDeclEnd(allToStore);
}

void RecurDec::gen() {
	this->genAstForwardDecl();
	this->genRules();
	this->writeErrorStuff();
}
