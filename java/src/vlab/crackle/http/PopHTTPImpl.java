/// ------------------------------------------------------------------
/// Copyright (c) from 1996 Vincent Risi 
///
/// All rights reserved. 
/// This program and the accompanying materials are made available 
/// under the terms of the Common Public License v1.0 
/// which accompanies this distribution and is available at 
/// http://www.eclipse.org/legal/cpl-v10.html 
/// Contributors:
///    Vincent Risi
/// ------------------------------------------------------------------

package vlab.crackle.http;

import vlab.crackle.*;
import vlab.crackle.Module;
import static vlab.crackle.Writer.*;

import java.io.*;
import java.util.Vector;

public class PopHTTPImpl extends Generator
{
  private static final PrintWriter outLog = new PrintWriter(System.out);
  private static boolean hasPython = false;
  private static boolean hasShutDownCode = false;
  /**
   * Generates - C Header for Server - C Server JSON code
   */
  private static String database;

  /*
   * Static constructor
   */
  {
    setupPragmaVector();
  }

  public static String description()
  {
    return "Generates JSON Server Implementation Code";
  }

  public static String documentation()
  {
    return "Generates JSON Server Implementation Code";
  }

  private static void setupPragmaVector()
  {
    if (pragmaVector == null)
    {
      pragmaVector = new Vector<Pragma>();
    }
  }

  private static void setPragmas(Module module)
  {
    // Ensure these are in the same order as above
    setupPragmaVector();
  }

  public static void generate(Module module, String output, PrintWriter outLog)
  {
    logger = outLog;
    logln(module.name + " version " + module.version);
    setPragmas(module);
    loadProperties(module, output);
    database = getProperty("database", "dbportal");
    logln(format("database:%s", database));
    for (int i = 0; i < module.prototypes.size(); i++)
    {
      Prototype prototype = (Prototype) module.prototypes.elementAt(i);
      if (prototype.isPythonImpl() == true)
      {
        hasPython = true;
        break;
      }
    }
    generateCHeader(module, output);
    if (module.tables.size() > 0)
    {
      generateCTablesHeader(module, output);
      generateCTablesCode(module, output);
    }
    generateCCode(module, output);
  }

  private static void generateCTablesHeader(Module module, String output)
  {
    try
    {
      logln("Code: " + output + module.name.toLowerCase() + "tables.h");
      OutputStream outFile = new FileOutputStream(output + module.name.toLowerCase() + "tables.h");
      writer = new PrintWriter(outFile);
      try
      {
        writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln("#ifndef _" + module.name + "Tables_H_");
        writeln("#define _" + module.name + "Tables_H_");
        writeln("#include \"machine.h\"");
        writeln();
        for (int i = 0; i < module.tables.size(); i++)
        {
          Table table = (Table) module.tables.elementAt(i);
          writeln("enum e" + module.name + table.name);
          writeln("{");
          String comma = "  ";
          for (int j = 0; j < table.messages.size(); j++)
          {
            Message message = (Message) table.messages.elementAt(j);
            writeln(1, comma + table.name.toLowerCase() + message.name + "   // " + message.value);
            comma = ", ";
          }
          writeln(comma + table.name.toLowerCase() + "NoOf");
          writeln("};");
          writeln();
          writeln("extern char *" + module.name + table.name + "[];");
          writeln();
        }
        writeln("#endif");
      }
      finally
      {
        writer.flush();
        outFile.close();
      }
    }
    catch (IOException e1)
    {
      logln("Generate Procs IO Error");
      System.out.println(e1);
      System.out.flush();
      e1.printStackTrace();
    }
    catch (Throwable e)
    {
      System.out.println(e);
      System.out.flush();
      e.printStackTrace();
    }
  }

  private static void generateCTablesCode(Module module, String output)
  {
    try
    {
      logln("Code: " + output + module.name.toLowerCase() + "tables.cpp");
      OutputStream outFile = new FileOutputStream(output + module.name.toLowerCase() + "tables.cpp");
      writer = new PrintWriter(outFile);
      try
      {
        writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln("#include \"" + module.name.toLowerCase() + "tables.h\"");
        writeln();
        for (int i = 0; i < module.tables.size(); i++)
        {
          Table table = (Table) module.tables.elementAt(i);
          String comma = "  ";
          writeln("char *" + module.name + table.name + "[] = ");
          writeln("{");
          for (int j = 0; j < table.messages.size(); j++)
          {
            Message message = (Message) table.messages.elementAt(j);
            writeln(1, comma + message.value + "   // " + message.name);
            comma = ", ";
          }
          writeln("};");
          writeln();
        }
      }
      finally
      {
        writer.flush();
        outFile.close();
      }
    }
    catch (IOException e1)
    {
      logln("Generate Procs IO Error");
      System.out.println(e1);
      System.out.flush();
      e1.printStackTrace();
    }
    catch (Throwable e)
    {
      System.out.println(e);
      System.out.flush();
      e.printStackTrace();
    }
  }

  private static String cleanUp(String data)
  {
    if (data.indexOf("HEADER:") == 0)
      return data.substring(7);
    if (data.indexOf("CODE:") == 0)
      return data.substring(5);
    if (data.indexOf("BOTH:") == 0)
      return data.substring(5);
    return data;
  }

  private static void generateCHeader(Module module, String output) //, PrintWriter outLog)
  {
    try
    {
      logln("Code: " + output + module.name.toLowerCase() + ".h");
      OutputStream outFile = new FileOutputStream(output + module.name.toLowerCase() + ".h");
      writer = new PrintWriter(outFile);
      try
      {
        String upperModule = module.name.toUpperCase();
        boolean hasProtected = false;
        boolean hasSlackHandler = false;
        writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln("#ifndef _" + module.name + "H_");
        writeln("#define _" + module.name + "H_");
        writeln("#include \"machine.h\"");
        writeln("#include \"logfile.h\"");
        writeln("#include \"ini.h\"");
        writeln("#include \"json/json.h\"");
        writeln("using namespace Json;");
        writeln("#include \"json_databuild.h\"");
        if (database.equals("dbportal"))
        {
          writeln("#define DATABASE_DBPORTAL");
          writeln("#include \"popgen.h\"");
          writeln("#include \"dbportal.h\"");
        } else if (database.equals("cliapi"))
        {
          writeln("#define DATABASE_CLIAPI");
          writeln("#include \"padgen.h\"");
          writeln("#include \"cliapi.h\"");
        } else if (database.equals("ociapi"))
        {
          writeln("#define DATABASE_OCIAPI");
          writeln("#include \"padgen.h\"");
          writeln("#include \"ociapi.h\"");
        } else
        {
          writeln("#define DATABASE_NONE");
        }
        writeln();
        PopGen.generateCTableAndEnums(module, writer, true);
        for (int i = 0; i < module.prototypes.size(); i++)
        {
          Prototype prototype = (Prototype) module.prototypes.elementAt(i);
          if (prototype.name.equals("SlackTimeHandler") == true)
          {
            hasSlackHandler = true;
            break;
          }
        }
        if (module.code.size() > 0)
        {
          boolean firstTime = true;
          for (int j = 0; j < module.code.size(); j++)
          {
            String codeLine = (String) module.code.elementAt(j);
            if (codeLine.indexOf("PYTHON:") >= 0)
              break;
            boolean outputIt = false;
            // #include go to header - unless flagged as CODE
            if (codeLine.indexOf("#include") >= 0 && codeLine.indexOf("CODE:") < 0)
              outputIt = true;
              // #other code goes to header if flagged as BOTH or HEADER
            else if (codeLine.indexOf("HEADER:") >= 0 || codeLine.indexOf("BOTH:") >= 0)
              outputIt = true;
            if (outputIt == true)
            {
              if (firstTime)
                firstTime = false;
              write(cleanUp(codeLine));
            }
          }
          if (firstTime == false)
            writeln();
        }
        PopGen.generateCExterns(module, writer);
        writeln("extern char *" + module.name + "Version;");
        writeln("extern int32 " + module.name + "Signature;");
        writeln("extern char *ConfigFile;");
        writeln();
        PopGen.generateCStructs(module, writer, true);
        writeln();
        writeln("#  if defined HTTP_SERVER");
        writeln("#include \"httpserver.h\"");
        writeln("class HTTPServer;");
        if (hasSlackHandler == true)
          writeln("void SlackHandler(void *);");
        writeln("#  endif");
        writeln("class t" + module.name);
        writeln("{");
        writeln("private:");
        writeln("#  if defined HTTP_SERVER");
        writeln(1, "HTTPServer *RPC;");
        writeln("#  endif");
        writeln();
        for (int i = 0; i < module.structures.size(); i++)
        {
          Structure structure = (Structure) module.structures.elementAt(i);
          if (structure.codeType == Structure.PROTECTED)
            hasProtected = true;
          if (structure.codeType != Structure.PRIVATE)
            continue;
          for (int j = 0; j < structure.fields.size(); j++)
          {
            Field field = (Field) structure.fields.elementAt(j);
            writeln(1, field.type.cDef(field.name, false) + ";");
          }
        }
        for (int i = 0; i < module.prototypes.size(); i++)
        {
          Prototype prototype = (Prototype) module.prototypes.elementAt(i);
          if (prototype.codeType == Prototype.PROTECTED)
            hasProtected = true;
          if (prototype.codeType != Prototype.PRIVATE)
            continue;
          PopGen.generateCHeader(module, prototype, writer);
        }
        writeln("public:");
        writeln(1, "tLogFile   &LogFile;");
        for (int i = 0; i < module.structures.size(); i++)
        {
          Structure structure = (Structure) module.structures.elementAt(i);
          if (structure.codeType != Structure.PUBLIC)
            continue;
          for (int j = 0; j < structure.fields.size(); j++)
          {
            Field field = (Field) structure.fields.elementAt(j);
            writeln(1, field.type.cDef(field.name, false) + ";");
          }
        }
        writeln(1, format("t%s(tLogFile& aLogFile)", module.name));
        writeln(2, ": LogFile(aLogFile)");
        writeln(1, "{");
        writeln(1, "}");
        writeln();
        writeln("#  if defined HTTP_SERVER");
        writeln(1, "void StartUpCode(HTTPServer* aRPC)");
        writeln(1, "{");
        writeln(2, "RPC = aRPC;");
        writeln("#  else");
        writeln(1, "void StartUpCode()");
        writeln(1, "{");
        writeln("#  endif");
        boolean closeEndif = false;
        for (int i = 0; i < module.structures.size(); i++)
        {
          Structure structure = (Structure) module.structures.elementAt(i);
          if (structure.codeType == Structure.NORMAL)
            continue;
          for (int j = 0; j < structure.code.size(); j++)
          {
            String codeLine = (String) structure.code.elementAt(j);
            if ((codeLine.trim()).equalsIgnoreCase("inall:"))
            {
              if (closeEndif == true)
              {
                closeEndif = false;
                writeln("#  endif");
                writeln();
              }
              continue;
            }
            if ((codeLine.trim()).equalsIgnoreCase("intcp:"))
            {
              if (closeEndif == true)
              {
                closeEndif = false;
                writeln("#  endif");
                writeln();
              }
              //state=in_TCPIP;
              writeln(format("#  if !defined %s_STANDALONE", upperModule));
              closeEndif = true;
              continue;
            }
            if ((codeLine.trim()).equalsIgnoreCase("inhttp:"))
            {
              if (closeEndif == true)
              {
                closeEndif = false;
                writeln("#  endif");
                writeln();
              }
              //state=in_HTTP;
              writeln("#  if defined HTTP_SERVER");
              closeEndif = true;
              continue;
            }
            if ((codeLine.trim()).equalsIgnoreCase("onshutdown:"))
            {
              if (closeEndif == true)
              {
                closeEndif = false;
                writeln("#  endif");
                writeln();
              }
              //state = in_ALL;
              writeln(1, "}");
              writeln(1, "void ShutDownCode()");
              writeln(1, "{");
              hasShutDownCode = true;
            } else
              write(1, codeLine);
          }
          if (closeEndif == true)
          {
            closeEndif = false;
            writeln("#  endif");
            writeln();
          }
          //state = in_ALL;
          writeln(1, "}");
        }
        for (int i = 0; i < module.prototypes.size(); i++)
        {
          Prototype prototype = (Prototype) module.prototypes.elementAt(i);
          if (prototype.codeType == Prototype.PRIVATE || prototype.codeType == Prototype.PROTECTED)
            continue;
          PopGen.generateCHeader(module, prototype, writer);
        }
        if (hasPython == true)
          writeln(1, "void (*runPython)(string scriptName, string input, string &output);");
        if (hasProtected == true)
        {
          writeln("protected:");
          for (int i = 0; i < module.structures.size(); i++)
          {
            Structure structure = (Structure) module.structures.elementAt(i);
            if (structure.codeType != Structure.PROTECTED)
              continue;
            for (int j = 0; j < structure.fields.size(); j++)
            {
              Field field = (Field) structure.fields.elementAt(j);
              writeln(1, field.type.cDef(field.name, false) + ";");
            }
          }
          for (int i = 0; i < module.prototypes.size(); i++)
          {
            Prototype prototype = (Prototype) module.prototypes.elementAt(i);
            if (prototype.codeType != Prototype.PROTECTED)
              continue;
            PopGen.generateCHeader(module, prototype, writer);
          }
        }
        writeln("};");
        writeln();
        writeln("#  if !defined " + upperModule + "_STANDALONE");
        writeln("class t" + module.name + "Server : public tSnapProcess");
        writeln("{");
        writeln("public:");
        writeln(1, "t" + module.name + "Server(tString IniFile);");
        writeln(1, "~t" + module.name + "Server();");
        writeln(1, "void ServiceClient();");
        writeln(1, "t" + module.name + " *" + module.name + ";");
        writeln(1, "tRPCServer *RPC;");
        writeln(1, "#ifdef UPM_AWARE");
        writeln(1, "tString UserID;");
        writeln(1, "tString HostName;");
        writeln(1, "#endif");
        writeln(1, "char *ReplyBody;");
        writeln(1, "int32 Result;");
        writeln(1, "int32 ReplySize;");
        writeln(1, "int32 RecvSize;");
        writeln(1, "void ReadIniFile(tString IniFile);");
        for (int i = 0; i < module.prototypes.size(); i++)
        {
          Prototype prototype = (Prototype) module.prototypes.elementAt(i);
          if (prototype.codeType != Prototype.RPCCALL)
            continue;
          writeln(1, "e" + module.name + " " + prototype.name + "(char *ip);");
        }
        writeln("};");
        writeln("#  endif");
        writeln();
        writeln("#endif");
      }
      finally
      {
        writer.flush();
        outFile.close();
      }
    }
    catch (IOException e1)
    {
      logln("Generate Procs IO Error");
      System.out.println(e1);
      System.out.flush();
      e1.printStackTrace();
    }
    catch (Throwable e)
    {
      System.out.println(e);
      System.out.flush();
      e.printStackTrace();
    }
  }

  private static void generateCCode(Module module, String output)
  {
    try
    {
      logln("Code: " + output + module.name.toLowerCase() + "impl_http.cpp");
      OutputStream outFile = new FileOutputStream(output + module.name.toLowerCase() + "impl_http.cpp");
      writer = new PrintWriter(outFile);
      try
      {
        String upperModule = module.name.toUpperCase();
        writeln("// This code was generated, do not modify it, modify it at source and regenerate it.");
        writeln();
        //writeln("#include \"ti.h\"");
        writeln("#include <stdio.h>");
        writeln("#include <stdlib.h>");
        writeln("#include <string.h>");
        writeln("#include <assert.h>");
        writeln();
        writeln("#include \"" + module.name.toLowerCase() + ".h\"");
        writeln();
        writeln("#ifdef DIM");
        writeln("#undef DIM");
        writeln("#endif");
        writeln();
        writeln("#define DIM(a)   (sizeof(a) / sizeof(a[0]))");
        writeln();
        writeln("const char *" + module.name + "Errors[] = ");
        writeln("{ \"No Error\"");
        for (int i = 0; i < module.messages.size(); i++)
        {
          Message message = (Message) module.messages.elementAt(i);
          writeln(", " + message.value + "   // " + message.name);
        }
        writeln(", \"Invalid Signature\"");
        writeln(", \"Invalid Logon Cookie\"");
        writeln(", \"Invalid INI File\"");
        writeln(", \"Uncaught DB Connect Error\"");
        writeln(", \"Unknown function call\"");
        writeln(", \"Snap Creation Error\"");
        writeln(", \"AddList (Index == 0) != (List == 0) Error\"");
        writeln(", \"AddList (realloc == 0) Error\"");
        writeln(", \"Last error not in message table\"");
        writeln("};");
        writeln();
        writeln("const char *" + module.name + "Error(int no)");
        writeln("{");
        writeln(1, "int size = DIM(" + module.name + "Errors);");
        writeln(1, "if (no == 0) return " + module.name + "Errors[0];");
        writeln(1, "int index = (no+1) - " + module.messageBase + ";");
        writeln(1, "if (index >= 0 && index < size)");
        writeln(2, "return " + module.name + "Errors[index];");
        writeln(1, "return \"<<error index out of range>>\";");
        writeln("}");
        writeln();
        writeln("TBChar " + upperModule + "_ErrBuffer;");
        writeln("#define THROW(err) throw " + upperModule + "_##err");
        writeln("#define THROW2(err, str) " + upperModule + "_ErrBuffer.set(str);throw " + module.name.toUpperCase() + "_##err");
        writeln("#define GETERRSTR " + upperModule + "_ErrBuffer.data");
        writeln();
        writeln("template <class TYPE, class INDEX>");
        writeln("void AddList(TYPE*& List, INDEX& Index, const TYPE& Rec, const INDEX Delta)");
        writeln("{");
        writeln(1, "if ((List == 0) != (Index == 0))");
        writeln(2, "THROW(ADDLIST_ERROR);");
        writeln(1, "if (Index % Delta == 0)");
        writeln(1, "{");
        writeln(2, "TYPE* Resized = (TYPE*) realloc(List, sizeof(Rec)*(Index+Delta));");
        writeln(2, "if (Resized == 0) // Cater for alloc failure");
        writeln(3, "THROW(ADDLIST_REALLOC_FAILED);");
        writeln(2, "List = Resized;");
        writeln(1, "}");
        writeln(1, "List[Index++] = Rec;");
        writeln("}");
        writeln();
        if (module.code.size() > 0)
        {
          boolean firstTime = true;
          for (int j = 0; j < module.code.size(); j++)
          {
            String codeLine = (String) module.code.elementAt(j);
            boolean outputIt = true;
            if (codeLine.indexOf("PYTHON:") >= 0)
              break;
            if (codeLine.indexOf("HEADER:") >= 0)
              outputIt = false;
            else if (codeLine.indexOf("BOTH:") >= 0 || codeLine.indexOf("CODE:") >= 0)
              ;// outputIt = true;
            else if (codeLine.indexOf("#include") >= 0)
              outputIt = false;
            if (firstTime == true)
              firstTime = false;
            if (outputIt == true)
              write(cleanUp(codeLine));
          }
          if (firstTime == false)
            writeln();
        }
        for (int i = 0; i < module.prototypes.size(); i++)
        {
          Prototype prototype = (Prototype) module.prototypes.elementAt(i);
          if (prototype.isPythonImpl() == true)
          {
            generateForPython(module, prototype);
            continue;
          }
          if (prototype.code.size() == 0)
            continue;
          PopGen.generateCImplCode(module, prototype, writer);
          writeln("{");
          for (int j = 0; j < prototype.code.size(); j++)
          {
            String codeLine = (String) prototype.code.elementAt(j);
            write(codeLine);
          }
          writeln("}");
          writeln();
        }
      }
      finally
      {
        writer.flush();
        outFile.close();
      }
    }
    catch (IOException e1)
    {
      logln("Generate Procs IO Error");
      System.out.println(e1);
      System.out.flush();
      e1.printStackTrace();
    }
    catch (Throwable e)
    {
      System.out.println(e);
      System.out.flush();
      e.printStackTrace();
    }
  }

  private static void generateForPython(Module module, Prototype prototype)
  {
    boolean hasInput = false;
    boolean hasOutput = false;
    boolean hasResult = false;
    for (int i = 0; i < prototype.inputs.size(); i++)
    {
      Action input = (Action) prototype.inputs.elementAt(i);
      Field field = input.getParameter(prototype);
      if (field == null)
        continue;
      hasInput = true;
      break;
    }
    if (prototype.type.typeof != Type.VOID)
    {
      if (prototype.type.reference != Type.BYPTR)
        hasResult = true;
      else
        hasOutput = true;
    }
    if (prototype.outputs.size() > 0)
      hasOutput = true;
    PopGen.generateCImplCode(module, prototype, writer);
    writeln("{");
    writeln(1, "if (runPython == 0)");
    if (hasResult)
      writeln(2, "return 0;");
    else
      writeln(2, "return;");
    writeln(1, "try");
    writeln(1, "{");
    if (hasResult)
      writeln(2, "int32 returnCode;");
    writeln(2, "string input, output;");
    writeln(2, "DataBuilderJson builder = DataBuilderJson();");
    if (hasInput)
      generateForPythonInputs(prototype);
    writeln(2, format("string scriptName = \"%s\";", prototype.name));
    writeln(2, "runPython(scriptName, input, output);");
    if (hasOutput || hasResult)
      generateForPythonOutputs(prototype, hasResult);
    writeln(1, "}");
    writeln(1, "catch (...)");
    writeln(1, "{");
    writeln(2, "TBChar errorBuf;");
    writeln(2, "PyErrorHandler(errorBuf);");
    writeln(2, "THROW2(PYTHON_ERROR, errorBuf.data);");
    writeln(1, "}");
    writeln("}");
    writeln();
  }

  private static void generateForPythonOutputs(Prototype prototype, boolean hasResult)
  {
    writeln(2, "Value output_value;");
    writeln(2, "stringstream sstr(output);");
    writeln(2, "sstr >> output_value;");
    if (hasResult)
      writeln(2, "returnCode = output_value[\"returnCode\"].asInt();");
    for (int i = 0; i < prototype.outputs.size(); i++)
    {
      Action action = (Action) prototype.outputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      boolean hasSize = action.hasSize();
      if (field.type.reference == Type.BYREFPTR)
      {
        if (field.type.typeof == Type.CHAR)
        {
          if (hasSize)
          {
            writeln(2, "//o BYREFPTR CHAR " + field.name);
            Operation op = action.sizeOperation();
            writeln(2, format("string %1$s_string = output_value[\"%1$s\"].asString();", field.name));
            writeln(2, format("%s = (char*) calloc(*%s + 1, 1);", field.name, op.name));
            writeln(2, format("memcpy(%1$s, %1$s_string.c_str(), *%2$s);", field.name, op.name));
          } else
            writeln(2, "//o BYREFPTR CHAR no size " + field.name);

          continue;
        }
        if (field.type.typeof == Type.USERTYPE)
        {
          if (hasSize)
          {
            writeln(2, "//o BYREFPTR USERTYPE " + field.name);
            Operation op = action.sizeOperation();
            writeln(2, format("%1$s = (%2$s*)calloc(*%3$s, sizeof(%2$s));", field.name, field.type.name, op.name));
            writeln(2, format("Value Recs_array = output_value[\"Recs\"];"));
            writeln(2, format("for (int i = 0; i < *%s; i++)", op.name));
            writeln(2, "{");
            writeln(3, format("Value %1$s_value = %1$s_array[i];", field.name));
            writeln(3, format("builder.setValue(%s_value);", field.name));
            writeln(3, format("%s[i].SetData(builder);", field.name));
            writeln(2, "}");
          } else
            writeln(2, "//o BYREFPTR USERTYPE no size " + field.name);
          continue;
        }
        writeln(2, "//o BYREFPTR STDTYPE " + field.name);
      } else if (field.type.reference == Type.BYPTR)
      {
        if (field.type.typeof == Type.CHAR)
        {
          writeln(1, "//o BYPTR CHAR " + field.name);
          continue;
        }
        if (field.type.typeof == Type.USERTYPE)
        {
          writeln(2, "//o BYPTR USERTYPE " + field.name);
          writeln(2, format("Value %1$s_value = output_value[\"%1$s\"];", field.name));
          writeln(2, format("builder.setValue(%s_value);", field.name));
          writeln(2, format("%s->SetData(builder);", field.name));
          continue;
        }
        writeln(2, "//o BYPTR STDTYPE " + field.name);
        writeln(2, format("*%s = output_value[\"%s\"]%s;", field.name, field.name, asType(field)));
      } else
      {
        writeln(2, "//o no ref " + field.name);
      }
    }
    if (hasResult)
      writeln(2, "return returnCode;");
  }

  private static void generateForPythonInputs(Prototype prototype)
  {
    writeln(2, "Value input_value;");
    for (int i = 0; i < prototype.inputs.size(); i++)
    {
      Action action = (Action) prototype.inputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      if (field.type.reference == Type.BYPTR)
      {
        if (action.hasSize() == false)
        {
          if (field.type.typeof == Type.CHAR)
          {
            writeln(2, "//i BYPTR CHAR no size " + field.name);
            writeln(2, format("input_value[\"%s\"] = %s;", field.name, field.name));
          } else if (field.type.typeof == Type.USERTYPE)
          {
            writeln(2, "//i BYPTR USERTYPE no size " + field.name);
            writeln(2, format("%s->BuildData(builder);", field.name));
            writeln(2, format("string %s_result;", field.name));
            writeln(2, format("input_value[\"%s\"] = builder.getRecord(%s_result);", field.name, field.name));
          } else
          {
            writeln(2, "//i BYPTR STDTYPE no size " + field.name);
            writeln(2, format("input_value[\"%s\"] = *%s;", field.name, field.name));
          }
        } else // has size
        {
          if (field.type.typeof == Type.CHAR)
          {
            writeln(2, format("//i BYPTR CHAR has size %s;", field.name));
          } else if (field.type.typeof == Type.USERTYPE)
          {
            writeln(2, format("//i BYPTR USERTYPE has size %s;", field.name));
            Operation op = action.sizeOperation();
            writeln(2, "Value " + field.name + "_array;");
            writeln(2, format("for (int i=0; i < %s; i++)", op.name));
            writeln(2, "{");
            makeValueRec(field.name, 3);
            writeln(3, "" + field.name + "_array[i] = " + field.name + "_out;");
            writeln(2, "}");
            writeln(2, format("input_value[\"%s\"] = %s_array;", field.name, field.name));
          } else
          {
            writeln(2, format("//i BYPTR STD has size %s;", field.name));
          }
        }
      } else if (field.type.reference == Type.BYREFPTR)
      {
        writeln(2, format("//i BYREFPTR %s;", field.name));
      } else
      {
        writeln(2, format("//i STDTYPE %s;", field.name));
        writeln(2, format("input_value[\"%s\"] = %s;", field.name, field.name));
      }
    }
    writeln(2, "StreamWriterBuilder swb;");
    writeln(2, "input = writeString(swb, input_value);");
  }

  private static void makeValueRec(String name, int no)
  {
    writeln(no, "Value " + name + "_out;");
    writeln(no, "" + name + "[i].BuildData(builder);");
    writeln(no, "string " + name + "_result;");
    writeln(no, "" + name + "_out = builder.getRecord(" + name + "_result);");
  }

  private static String asType(Field field)
  {
    switch (field.type.typeof)
    {
      case Type.CHAR:
      case Type.STRING:
        return ".asString()";
      case Type.BYTE:
      case Type.BOOLEAN:
      case Type.INT:
      case Type.SHORT:
        return ".asInt()";
      case Type.LONG:
        return ".asInt64()";
      case Type.FLOAT:
      case Type.DOUBLE:
        return ".asDouble()";
      default:
        break;
    }
    return "";
  }
}
