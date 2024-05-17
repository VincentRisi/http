package vlab.crackle.http;

import java.io.PrintWriter;
import vlab.crackle.Module;
import vlab.crackle.*;

public class PopGen
{
  public static PrintWriter errLog = new PrintWriter(System.out);

  /**
   * Sets up the writer and generates the general stuff
   */
  public static void generateCExterns(Module module, PrintWriter writer)
  {
    boolean done = false;
    for (int i = 0; i < module.structures.size(); i++)
    {
      boolean found = false;
      Structure struct1 = (Structure) module.structures.elementAt(i);
      if (struct1.codeType != Structure.NORMAL)
        continue;
      if (struct1.header.length() > 0)
      {
        for (int j = 0; j < i; j++)
        {
          Structure struct2 = (Structure) module.structures.elementAt(j);
          if (struct2.header.compareTo(struct1.header) == 0)
          {
            found = true;
            break;
          }
        }
        if (found == false)
        {
          writer.println("#include " + struct1.header);
          done = true;
        }
      }
    }
    if (done)
      writer.println();
  }
  
  private static boolean enumsGenerated = false;

  public static void generateCTableAndEnums(Module module, PrintWriter writer, boolean doSwap)
  {
    enumsGenerated = true;
    for (int i = 0; i < module.tables.size(); i++)
    {
      Table table = (Table) module.tables.elementAt(i);
      writer.println("enum e" + module.name + table.name);
      writer.println("{");
      String comma = "  ";
      for (int j = 0; j < table.messages.size(); j++)
      {
        Message message = (Message) table.messages.elementAt(j);
        writer.println(comma + table.name.toLowerCase() + message.name + "   // " + message.value);
        comma = ", ";
      }
      writer.println(comma + table.name.toLowerCase() + "NoOf");
      writer.println("};");
      writer.println();
      writer.println("extern const char *" + module.name + table.name + "[];");
      writer.println();
    }
    if (module.enumerators.size() > 0)
    {
      for (int i = 0; i < module.enumerators.size(); i++)
      {
        Enumerator entry = (Enumerator) module.enumerators.elementAt(i);
        writer.println("enum " + entry.name);
        String w1 = "{ ";
        for (int j = 0; j < entry.elements.size(); j++)
        {
          String element = (String) entry.elements.elementAt(j);
          writer.println(w1 + element);
          w1 = ", ";
        }
        writer.println("};");
        writer.println();
      }
    }
  }

  /**
   * Generates the data structures defined
   */
  public static void generateCStructs(Module module, PrintWriter writer, boolean doSwap)
  {
    generateCStructs(module, writer, doSwap, true);
  }
  
  /**
   * Generates the data structures defined
   */
  public static void generateCStructs(Module module, PrintWriter writer, boolean doSwap, boolean allMessages)
  {
    writer.println("enum e" + module.name);
    writer.println("{ " + module.name.toUpperCase() + "_OK");
    String w0 = "";
    if (module.messageBase > 0)
      w0 = " = " + module.messageBase;
    for (int i = 0; i < module.messages.size(); i++)
    {
      Message message = (Message) module.messages.elementAt(i);
      writer.println(", " + module.name.toUpperCase() + "_" + message.name + w0 + " // " + message.value);
      w0 = "";
    }
    writer.println(", " + module.name.toUpperCase() + "_INV_SIGNATURE");
    writer.println(", " + module.name.toUpperCase() + "_INV_COOKIE");
    if (allMessages)
    {
      writer.println(", " + module.name.toUpperCase() + "_INV_INIFILE");
      writer.println(", " + module.name.toUpperCase() + "_UNCAUGHT_DB_ERROR");
      writer.println(", " + module.name.toUpperCase() + "_UNKNOWN_FUNCTION");
      writer.println(", " + module.name.toUpperCase() + "_SNAP_ERROR");
      writer.println(", " + module.name.toUpperCase() + "_ADDLIST_ERROR");
      writer.println(", " + module.name.toUpperCase() + "_ADDLIST_REALLOC_FAILED");
    }
    writer.println(", " + module.name.toUpperCase() + "_LAST_LAST");
    writer.println("};");
    writer.println();
    writer.println("extern const char *" + module.name + "Errors[];");
    writer.println("const char *" + module.name + "Error(int no);");
    writer.println();
    if (enumsGenerated == false)
      generateCTableAndEnums(module, writer, doSwap);    
    for (int i = 0; i < module.structures.size(); i++)
    {
      Structure structure = (Structure) module.structures.elementAt(i);
      if (structure.codeType != Structure.NORMAL)
        continue;
      if (structure.fields.size() > 0)
      {
        writer.println("struct " + structure.name);
        writer.println("{");
        int relativeOffset = 0;
        int fillerNo = 0;
        for (int j = 0; j < structure.fields.size(); j++)
        {
          Field field = (Field) structure.fields.elementAt(j);
          int n = relativeOffset % field.type.paddingSize(true);
          if (n > 0)
          {
            n = field.type.paddingSize(true) - n;
            writer.println("  char filler" + fillerNo + "[" + n + "];");
            fillerNo++;
            relativeOffset += n;
          }
          writer.println("  " + field.type.cDef(field.name, false) + ";");
          relativeOffset += field.type.relativeSize(true);
        }
        int n = relativeOffset % 8;
        if (n > 0)
        {
          n = 8 - n;
          writer.println("  char filler" + fillerNo + "[" + n + "];");
        }
        writer.println("  " + structure.name + "()");
        writer.println("  {");
        writer.println("    memset(this, 0, sizeof(*this));");
        writer.println("  }");
        if (doSwap)
        {
          writer.println("  void Swaps()");
          writer.println("  {");
          for (int k = 0; k < structure.fields.size(); k++)
          {
            String w1 = "", w2 = "", w3 = ".", w4 = "", w5 = "";
            Field field = (Field) structure.fields.elementAt(k);
            if (field.type.reference == Type.BYPTR || field.type.reference == Type.BYREFPTR)
            {
              w3 = "->";
              w4 = "*";
            }
            if (field.needsSwap() || field.isStruct(module))
            {
              for (int j = 0; j < field.type.arraySizes.size(); j++)
              {
                Integer integer = (Integer) field.type.arraySizes.elementAt(j);
                writer.println(
                    "    " + w2 + "{for (int k" + j + " = 0; k" + j + " < " + integer.intValue() + "; k" + j + "++)");
                w1 = w1 + "[k" + j + "]";
                w2 = w2 + "  ";
                w5 = w5 + "}";
              }
            }
            if (field.needsSwap())
              writer.println("    " + w2 + "SwapBytes(" + w4 + field.name + w1 + ");" + w5);
            else if (field.isStruct(module))
              writer.println("    " + w2 + field.name + w1 + w3 + "Swaps();" + w5);
          }
          writer.println("  }");
          writer.println("  void Trims()");
          writer.println("  {");
          for (int k = 0; k < structure.fields.size(); k++)
          {
            String w1 = "", w2 = "", w3 = ".", w4 = "", w5 = "";
            Field field = (Field) structure.fields.elementAt(k);
            if (field.type.reference == Type.BYPTR || field.type.reference == Type.BYREFPTR)
            {
              w3 = "->";
              w4 = "*";
            }
            if (field.needsTrim() == true || field.isStruct(module))
            {

              for (int j = field.isStruct(module) ? 0 : 1; j < field.type.arraySizes.size(); j++)
              {
                Integer integer = (Integer) field.type.arraySizes.elementAt(j);
                writer.println(
                    "    " + w2 + "{for (int k" + j + " = 0; k" + j + " < " + integer.intValue() + "; k" + j + "++)");
                w1 = w1 + "[k" + j + "]";
                w2 = w2 + "  ";
                w5 = w5 + "}";
              }
            }
            if (w1.length() > 0)
              w4 = w4 + "&";
            if (field.isStruct(module))
              writer.println("    " + w2 + field.name + w1 + w3 + "Trims();" + w5);
            else if (field.needsTrim())
              writer.println("    " + w2 + "TrimTrailingBlanks(" + w4 + field.name + w1 + ", sizeof(" + w4 + field.name
                  + w1 + "));" + w5);
          }
          writer.println("  }");
        }
        boolean doDataBuild = true;
        for (int j = 0; j < structure.fields.size(); j++)
        {
          Field field = (Field) structure.fields.elementAt(j);
          if (field.type.typeof == Type.VOID
          ||  field.type.typeof == Type.STRING
          ||  field.type.typeof == Type.WCHAR
          )
          {
            writer.println("  // " + field.type.name + " not supported in data build");
            doDataBuild = false;
            break;
          }
          if (field.type.typeof != Type.CHAR
          && (field.type.reference == Type.BYPTR || field.type.reference == Type.BYREFPTR)
          )               
          {
            writer.println("  // " + field.type.name + " by pointer not supported in data build");
            doDataBuild = false;
            break;
          }
        }
        if (doDataBuild == true)
        {
          writer.println("#if defined(_DATABUILD_H_) || defined(_DATABUILD2_H_)");
          writer.println("  void BuildData(DataBuilder &dBuild, char *name=\""+structure.name+"\")");
          writer.println("  {");
          writer.println("    dBuild.name(name);");
          writer.println("    dBuild.count(" + structure.fields.size() + "); // requires _DATABUILD2_H_");
          for (int j = 0; j < structure.fields.size(); j++)
          {
            Field field = (Field) structure.fields.elementAt(j);
            String name = field.name;
            switch (field.type.typeof)
            {
            case Type.USERTYPE:
              writer.println("    " + field.name + ".BuildData(dBuild, \"" + field.type.name + "\");");
              break;
            case Type.CHAR:
              writer.println("    dBuild.add(\"" + name + "\", " + name + ", sizeof(" + name + "), \"\");");
              break;
            case Type.BYTE:
              if (field.type.arraySizes.size() > 0)
              {
                  writer.println("    dBuild.add(\"" + name + "\", (char *)" + name + ", sizeof(" + name + "), \"\");");
                  break;
              }
            case Type.BOOLEAN:
            case Type.INT:
            case Type.SHORT:
            case Type.LONG:
            case Type.FLOAT:
            case Type.DOUBLE:
              writer.println("    dBuild.add(\"" + name + "\", " + name + ", \"\");");
              break;
            case Type.VOID:
              break;
            case Type.STRING:
              break;
            case Type.WCHAR:
              break;
            default:
              break;
            }
          }
          writer.println("  }");
          writer.println("  void SetData(DataBuilder &dBuild, char *name=\""+structure.name+"\")");
          writer.println("  {");
          writer.println("    dBuild.name(name);");
          writer.println("    dBuild.count(" + structure.fields.size() + "); // requires _DATABUILD2_H_");
          for (int j = 0; j < structure.fields.size(); j++)
          {
            Field field = (Field) structure.fields.elementAt(j);
            String name = field.name;
            switch (field.type.typeof)
            {
            case Type.USERTYPE:
              writer.println("    " + field.name + ".SetData(dBuild, \"" + field.type.name + "\");");
              break;
            case Type.CHAR:
              writer.println("    dBuild.set(\"" + name + "\", " + name + ", sizeof(" + name + "), \"\");");
              break;
            case Type.BYTE:
            case Type.BOOLEAN:
              writer.println("    dBuild.set(\"" + name + "\", " + name + ", sizeof(" + name + "), \"\");");
              break;
            case Type.INT:
            case Type.SHORT:
            case Type.LONG:
            case Type.FLOAT:
            case Type.DOUBLE:
              writer.println("    dBuild.set(\"" + name + "\", " + name + ", sizeof(" + name + "), \"\");");
              break;
            case Type.VOID:
              break;
            case Type.STRING:
              break;
            case Type.WCHAR:
              break;
            default:
              break;
            }
          }
          writer.println("  }");
          writer.println("#endif");
        }
        writer.println("};");
        writer.println();
      }
    }
  }

  /**
   * Generates the actions
   */
  private static void generateCActions(Module module, Prototype prototype, PrintWriter writer, String pad)
  {
    writer.println(pad + "/**");
    if (prototype.inputs.size() > 0)
    {
      writer.println(pad + "* INPUT:");
      for (int i = 0; i < prototype.inputs.size(); i++)
      {
        Action input = (Action) prototype.inputs.elementAt(i);
        Operation op = input.sizeOperation();
        writer.println(pad + "*   " + input.name + ((op != null) ? " " + op.name : "") + ";");
      }
    }
    if (prototype.outputs.size() > 0)
    {
      writer.println(pad + "* OUTPUT:");
      for (int i = 0; i < prototype.outputs.size(); i++)
      {
        Action output = (Action) prototype.outputs.elementAt(i);
        Operation op = output.sizeOperation();
        writer.println(pad + "*   " + output.name + ((op != null) ? " " + op.name : "") + ";");
      }
    }
    writer.println(pad + "* RETURNS: " + prototype.type.cName(false));
    writer.println(pad + "*/");
  }

  /**
   * Generates the prototypes defined
   */
  public static void generateCHeader(Module module, Prototype prototype, PrintWriter writer)
  {
    generateCActions(module, prototype, writer, "  ");
    String w1 = "";
    writer.print("  " + prototype.type.cDef(prototype.name, false) + "(");
    for (int i = 0; i < prototype.parameters.size(); i++)
    {
      Field parameter = (Field) prototype.parameters.elementAt(i);
      writer.print(w1 + parameter.type.cDef(parameter.name, false));
      w1 = ", ";
    }
    writer.println(");");
  }

  /**
   * Generates the prototypes defined
   */
  public static void generateCInterface(Module module, Prototype prototype, PrintWriter writer)
  {
    generateCActions(module, prototype, writer, "  ");
    String w1 = "";
    writer.print("  virtual " + prototype.type.cDef(prototype.name, false) + "(");
    for (int i = 0; i < prototype.parameters.size(); i++)
    {
      Field parameter = (Field) prototype.parameters.elementAt(i);
      writer.print(w1 + parameter.type.cDef(parameter.name, false));
      w1 = ", ";
    }
    writer.println(") = 0;");
  }

  /**
   * Generates the prototypes defined
   */
  public static void generateVirtualCHeader(Module module, Prototype prototype, PrintWriter writer)
  {
    generateCActions(module, prototype, writer, "  ");
    String w1 = "";
    writer.print("  virtual " + prototype.type.cDef(prototype.name, false) + "(");
    for (int i = 0; i < prototype.parameters.size(); i++)
    {
      Field parameter = (Field) prototype.parameters.elementAt(i);
      writer.print(w1 + parameter.type.cDef(parameter.name, false));
      w1 = ", ";
    }
    writer.println(") = 0;");
  }

  /**
   * Generates the implementation code
   */
  public static void generateCImplCode(Module module, Prototype prototype, PrintWriter writer)
  {
    generateCActions(module, prototype, writer, "");
    String w1 = "";
    writer.print(prototype.type.cDef("t" + module.name + "::" + prototype.name, false) + "(");
    for (int i = 0; i < prototype.parameters.size(); i++)
    {
      Field parameter = (Field) prototype.parameters.elementAt(i);
      writer.print(w1 + parameter.type.cDef(parameter.name, false));
      w1 = ", ";
    }
    writer.println(")");
  }

  /**
   * Generates the implementation code
   */
  public static void generateUbiCImplCode(Module module, Prototype prototype, PrintWriter writer)
  {
    generateCActions(module, prototype, writer, "");
    String w1 = "";
    writer.print(prototype.type.cDef("T" + module.name + "::" + prototype.name, false) + "(");
    for (int i = 0; i < prototype.parameters.size(); i++)
    {
      Field parameter = (Field) prototype.parameters.elementAt(i);
      writer.print(w1 + parameter.type.cDef(parameter.name, false));
      w1 = ", ";
    }
    writer.println(")");
  }

  /**
   * Generates the prototypes defined
   */
  public static void generateCSwaps(Module module, Prototype prototype, Field field, Operation op, PrintWriter writer)
  {
    String w1 = "", w2 = "", w3 = ".", w4 = "", w5 = "", w6 = "";
    int n = 0;
    if (field.needsSwap() || field.isStruct(module))
    {
      if (field.type.reference == Type.BYPTR || field.type.reference == Type.BYREFPTR)
      {
        w3 = "->";
        w4 = "*";
      }
      if (op != null)
      {
        Field opField = prototype.getParameter(op.name);
        if (opField != null && (opField.type.reference == Type.BYPTR || opField.type.reference == Type.BYREFPTR))
          w5 = "*";
        n++;
        writer.println("    " + w2 + "{for (int _i_" + n + op.name + " = 0; _i_" + n + op.name + " < " + w5 + op.name
            + "; _i_" + n + op.name + "++)");
        w1 = w1 + "[_i_" + n + op.name + "]";
        w2 = w2 + "  ";
        if (field.type.reference == Type.BYPTR)
          w3 = ".";
        w4 = "";
        w6 = w6 + "}";
      }
      for (int j = 0; j < field.type.arraySizes.size(); j++)
      {
        Integer integer = (Integer) field.type.arraySizes.elementAt(j);
        n++;
        writer.println("    " + w2 + "{for (int i_" + n + j + " = 0; i_" + n + j + " < " + integer.intValue() + "; i_"
            + n + j + "++)");
        w1 = w1 + "[i_" + n + j + "]";
        w2 = w2 + "  ";
        w3 = ".";
        w4 = "";
        w6 = w6 + "}";
      }
    }
    if (field.isStruct(module))
      writer.println("    " + w2 + field.name + w1 + w3 + "Swaps();" + w6);
    else if (field.needsSwap())
      writer.println("    " + w2 + "SwapBytes(" + w4 + field.name + w1 + ");" + w6);
    else if (field.type.typeof == Type.USERTYPE)
    {
      // Toolkit.getDefaultToolkit().beep();
      errLog.println("Warning: " + prototype.name + " " + field.name + " is of UserType and may require swapping.");
    }
  }

  /**
   * Generates the prototypes defined
   */
  public static void generateCTrims(Module module, Prototype prototype, Field field, Operation op, PrintWriter writer)
  {
    String w1 = "", w2 = "", w3 = ".", w4 = "", w5 = "", w6 = "";
    int n = 0;
    if (field.needsTrim() == true || field.isStruct(module))
    {
      if (field.type.reference == Type.BYPTR || field.type.reference == Type.BYREFPTR)
      {
        w3 = "->";
        w4 = "*";
      }
      if (op != null)
      {
        Field opField = prototype.getParameter(op.name);
        if (opField != null && (opField.type.reference == Type.BYPTR || opField.type.reference == Type.BYREFPTR))
          w5 = "*";
        n++;
        writer.println("    " + w2 + "{for (int _i_" + n + op.name + " = 0; _i_" + n + op.name + " < " + w5 + op.name
            + "; _i_" + n + op.name + "++)");
        w1 = w1 + "[_i_" + n + op.name + "]";
        w2 = w2 + "  ";
        if (field.type.reference == Type.BYPTR)
          w3 = ".";
        w4 = "";
        w6 = w6 + "}";
      }
      for (int j = 1; j < field.type.arraySizes.size(); j++)
      {
        Integer integer = (Integer) field.type.arraySizes.elementAt(j);
        n++;
        writer.println("    " + w2 + "{for (int i_" + n + j + " = 0; i_" + n + j + " < " + integer.intValue() + "; i_"
            + n + j + "++)");
        w1 = w1 + "[i_" + n + j + "]";
        w2 = w2 + "  ";
        w3 = ".";
        w4 = "";
        w6 = w6 + "}";
      }
    }
    if (field.isStruct(module))
      writer.println("    " + w2 + field.name + w1 + w3 + "Trims();" + w6);
    else if (field.needsTrim())
      writer.println(
          "    " + w2 + "TrimTrailingBlanks(" + w4 + field.name + w1 + ", sizeof(" + w4 + field.name + w1 + "));" + w6);
    else if (field.type.typeof == Type.USERTYPE)
    {
      // Toolkit.getDefaultToolkit().beep();
      errLog.println("Warning: " + prototype.name + " " + field.name + " is of UserType and may require swapping.");
    }
  }

  public static void generateCStructSwaps(Module module, Prototype prototype, Field field, Operation op,
      PrintWriter writer)
  {
    if (field.isStruct(module) == true)
    {
      generateCSwaps(module, prototype, field, op, writer);
      generateCTrims(module, prototype, field, op, writer);
    }
  }

  public static void generateCNonStructSwaps(Module module, Prototype prototype, Field field, Operation op,
      PrintWriter writer)
  {
    if (field.isStruct(module) == false)
    {
      generateCSwaps(module, prototype, field, op, writer);
      generateCTrims(module, prototype, field, op, writer);
    }
  }
}
