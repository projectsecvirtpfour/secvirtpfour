#include "../include/pcheaders.h"
#include "parser.tab.h"
#include "../include/AST.h"
#include "../include/SymbolTable.h"
#include "../include/CFG.h"
#include "../include/IPLBase.hpp"
#include "../include/PreProcessor.h"

extern AST* ast;
extern FILE *yyin;
extern void reset();

void parseArgs(const int argc, char **argv, std::vector<std::string> *P4Programs, bool *help, char portFile[]){
  int i = 1;
  while(i < argc){
    if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
      *help = true;
    }
    else if(!strcmp(argv[i], "--ports")){
      i++;
      if(i < argc)
        strncpy(portFile, argv[i], BUFFER_SIZE);
    }
    else
      P4Programs->push_back(std::string(argv[i]));
    i++;
  }
}

void printHelp(char *progName){
  printf("Usage: %s [Options] [P4programs, ...]\n", progName);
  printf("Options:\n");
  printf("(--ports FILE) | Specify the file containing the assigment of ports to programs.\n");
}

std::vector<std::set<int>> getVSPorts(char fileName[]){
  std::vector<std::set<int>> result;
  std::set<int> tempS;

  FILE *fp = fopen(fileName, "r");
  if(!fp){
    fprintf(stderr, "Error: Could not open file \"%s\"\n", fileName);
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  int bufferI = 0;

  while(!feof(fp)){
    char c = fgetc(fp);
    if(c == '\n' || c == ' ' || feof(fp)){
      if(bufferI > 0){
        buffer[bufferI] = '\0';
        bufferI = 0;
        int portN = IPLBase::intFromStr(buffer);
        if(portN == -1){
          fprintf(stderr, "Error: Malformed ports file \"%s\"\n", fileName);
          fclose(fp);
          exit(1);
        }
        tempS.emplace(portN);
      }
      if(c == '\n' && tempS.size() > 0){
        result.push_back(tempS);
        tempS.clear();
      }
    }
    else{
      buffer[bufferI] = c;
      bufferI++;
    }
  }

  fclose(fp);

  return result;
}

int main (int argc, char **argv)
{
  char buffer[BUFFER_SIZE];
  bool help = false;
  char portFile[BUFFER_SIZE] = "NO_FILE";
  P4Architecture programsArchitecture = P4A_UNDEFINED;
  std::vector<std::string> P4IncludePaths = {"p4include/"};
  std::vector<std::string> P4Programs;

  parseArgs(argc, argv, &P4Programs, &help, portFile);

  if(help || P4Programs.size() == 0){
    printHelp(argv[0]);
    return 0;
  }

  if(!strcmp(portFile, "NO_FILE")){
    fprintf(stderr, "Error: No port file provided. Use option --ports FILE.\n");
    return 1;
  }

  const std::vector<std::set<int>> virtualSwitchPorts = getVSPorts(portFile);
  if(virtualSwitchPorts.size() != P4Programs.size()){
    fprintf(stderr, "Error: Number of programs provided (%d) is different from the port specifications from file \"%s\" (%d)\n", (int) P4Programs.size(), portFile, (int) virtualSwitchPorts.size());
    return 1;
  }

  //PreProcessor
  for(int i = 0; i < (int) P4Programs.size(); i++){
    PreProcessor pp(P4Programs[i], P4IncludePaths);
    pp.Process();
    sprintf(buffer, "output/ir%d.p4", i + 1);
    FILE *fir = fopen(buffer, "w");
    pp.Print(fir);
    fclose(fir);

    if(i == 0)
      programsArchitecture = pp.getArch();
    
    if(pp.getArch() == P4A_UNDEFINED){
      fprintf(stderr, "Error: Could not detect architecture of program \"%s\"\n", P4Programs[i].c_str());
      return 1;
    }
    if(pp.getArch() != programsArchitecture){
      fprintf(stderr, "Error: Different program architectures detected\n");
      return 1;
    }
  }

  std::vector<AST*> ProgramASTs;
  for(int i = 0; i < (int) P4Programs.size(); i++){
    sprintf(buffer, "output/ir%d.p4", i + 1);
    FILE *fin = fopen(buffer, "r");
    if(!fin){
      fprintf(stderr, "Could not open file \"%s\"\n", P4Programs[i].c_str());
      return 1;
    }
    yyin = fin;
    int ret = yyparse();
    fclose(fin);
    if(ret != 0){
      return ret;
    }

    ast->Simplify();
    ast->SubstituteTypedef();

    ast->setValue(P4Programs[i].c_str());
    ProgramASTs.push_back(ast->Clone());
    delete ast;
    reset();
  }

  CFG::SetProgramNames(P4Programs);
  CFG::SetProgramASTs(ProgramASTs);
  CFG::SetArchitecture(programsArchitecture);

  for(int i = 0; i < (int) P4Programs.size(); i++){
    sprintf(buffer, "output/P%d-AST.dot", i+1);
    FILE *fpast = fopen(buffer, "w");
    ProgramASTs[i]->GenDot(fpast);
    fclose(fpast);
  }

  if(ProgramASTs.size() > 1){
    AST *merged = AST::MergeAST(ProgramASTs, virtualSwitchPorts, programsArchitecture);
    merged->RemoveErrorAndMKDeclarations();

    FILE *fp = fopen("output/merged.dot", "w");
    merged->GenDot(fp);
    fclose(fp);

    FILE *fmerged = fopen("output/merged.p4", "w");
    merged->writeCode(fmerged);
    fclose(fmerged);

    CFG::GenerateP4Infos(merged);

    for(AST *declaration : merged->getChildren())
      if(declaration->getNodeType() == NT_PARSER_DECLARATION)
        declaration->WriteParserDOT("output/" + declaration->getChildNT(NT_PARSER_TYPE_DECLARATION)->getValue() + ".dot");
    
    delete merged;
  }

  for(AST *ast : ProgramASTs)
    delete ast;

  return 0;
}
