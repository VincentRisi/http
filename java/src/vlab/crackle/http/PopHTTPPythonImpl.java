package vlab.crackle.http;

import java.io.FileOutputStream;
import java.io.PrintWriter;

import vlab.crackle.Module;
import vlab.crackle.*;

import static vlab.crackle.Writer.*;

import java.io.*;
import java.util.Vector;

public class PopHTTPPythonImpl extends Generator
{
  public static String description()
  {
    return "Generates Server Code (AIX|SUN|NT)";
  }

  public static String documentation()
  {
    return "Generates Python Code (AIX|LINUX|WINDOWS)";
  }

  /*
   * Static constructor
   */
  {
    setupPragmaVector();
  }

  private static void setupPragmaVector()
  {
    if (pragmaVector == null)
    {
      pragmaVector = new Vector<Pragma>();
    }
  }

  private static boolean hasPython = false;
  private static boolean hasIDLStructs = false;
  private static final Vector<Schema> schemas = new Vector<Schema>();
  private static final Vector<Method> methods = new Vector<Method>();

  private static void setPragmas(Module module)
  {
    setupPragmaVector();
  }

  public static void generate(Module module, String output, PrintWriter outLog)
  {
    logger = outLog;
    logln(module.name + " version " + module.version);
    setPragmas(module);
    for (int i = 0; i < module.prototypes.size(); i++)
    {
      Prototype prototype = (Prototype) module.prototypes.elementAt(i);
      if (prototype.isPythonImpl() == true)
      {
        hasPython = true;
        break;
      }
    }
    if (hasPython == false)
      return;
    buildSchemas(module);
    gatherMethods(module);
    generateStructs(module, output);
    generatePythonJSON(module, output);
    generateImplement(module, output);
  }

  private static void buildSchemas(Module module)
  {
    for (int i = 0; i < module.structures.size(); i++)
    {
      Structure structure = (Structure) module.structures.elementAt(i);
      if (structure.codeType != Structure.NORMAL)
        continue;
      String name = structure.name;
      boolean seen = false;
      for (int j = 0; j < schemas.size(); j++)
      {
        Schema schema = schemas.elementAt(j);
        if (schema.name.compareTo(name) == 0)
        {
          seen = true;
          break;
        }
      }
      if (seen == true)
        break;
      String header = module.name.toLowerCase();
      byte type = Schema.IDL_STRUCT;
      if (structure.fields.size() == 0)
      {
        String table = structure.header.substring(1);
        int end = table.indexOf('.');
        header = table.substring(0, end);
        type = Schema.DB_STRUCT;
      } else
        hasIDLStructs = true;
      Schema schema = new Schema(name, header, type, structure);
      schemas.addElement(schema);
    }
  }

  private static void gatherMethods(Module module)
  {
    for (int i = 0; i < module.prototypes.size(); i++)
    {
      Prototype prototype = (Prototype) module.prototypes.elementAt(i);
      if (prototype.isPythonImpl() == true)
      {
        Method method = new Method(prototype);
        methods.addElement(method);
        for (int j = 0; j < prototype.parameters.size(); j++)
        {
          Field field = (Field) prototype.parameters.elementAt(j);
          if (field.type.typeof == Type.USERTYPE)
          {
            String typeName = field.type.name;
            markUsed(typeName);
          }
        }
      }
    }
  }

  private static void markUsed(String typeName)
  {
    for (int i = 0; i < schemas.size(); i++)
    {
      Schema schema = schemas.elementAt(i);
      if (schema.name.compareTo(typeName) == 0)
        schema.used = true;
    }
  }

  private static void generateStructs(Module module, String output)
  {
    String comma = "";
    String fName = "%s%s_structs.py".formatted(output, module.name.toLowerCase());
    logln(format("Code: %s", fName));
    try (var outData = new PrintWriter(new FileOutputStream(fName)))
    {
      writer = outData;
      indent_size = 4;
      writeln(format("'''%s idl2 structures", module.name.toLowerCase()));
      writeln(1, "This code was generated, do not modify it, modify it at source and regenerate it.");
      writeln("'''");
      generateMessages(module);
      generateTables(module);
      generateEnumerators(module);
      generateImports(module, true);
      if (hasIDLStructs)
      {
        for (int i = 0; i < module.structures.size(); i++)
        {
          Structure structure = (Structure) module.structures.elementAt(i);
          if (structure.codeType != Structure.NORMAL)
            continue;
          if (structure.fields.size() > 0)
          {
            writeln();
            String name = format("t%s", structure.name.substring(1));
            writeln(format("class %s(object):", name));
            write(1, "__slots__ = [");
            comma = "";
            for (int j = 0; j < structure.fields.size(); j++)
            {
              Field field = (Field) structure.fields.elementAt(j);
              write(format("%s'%s'", comma, field.name));
              comma = ", ";
            }
            writeln("]");
            writeln(1, "def __init__(self):");
            for (int j = 0; j < structure.fields.size(); j++)
            {
              Field field = (Field) structure.fields.elementAt(j);
              writeln(2, format("self.%s = %s", field.name, getInit(field)));
            }
            writeln(1, "def _fields(self):");
            write(2, "return [");
            comma = "";
            for (int j = 0; j < structure.fields.size(); j++)
            {
              Field field = (Field) structure.fields.elementAt(j);
              write(format("%s'%s'", comma, field.name));
              comma = ", ";
            }
            writeln("]");
          }
        }
      }
    }
    catch (FileNotFoundException e)
    {
      e.printStackTrace();
    }
  }

  private static void generateMessages(Module module)
  {
    int base = module.messageBase;
    String prefix = module.name.toUpperCase();
    writeln("messages = {}");
    writeln(format("%1$s_OK = 0\nmessages[%1$s_OK] = 'No Error'", prefix));
    for (int i = 0; i < module.messages.size(); i++)
    {
      Message message = (Message) module.messages.elementAt(i);
      writeln(format("%s_%s = %d", prefix, message.name, base + i));
      writeln(format("messages[%s_%s] = %s", prefix, message.name, message.value));
    }
    writeln();
  }

  private static void generateTables(Module module)
  {
    for (int i = 0; i < module.tables.size(); i++)
    {
      Table table = (Table) module.tables.elementAt(i);
      String prefix = format("%s", table.name.toLowerCase());
      String var = format("%s%s", module.name.toLowerCase(), table.name);
      writeln(format("%s = {}", var));
      for (int j = 0; j < table.messages.size(); j++)
      {
        Message message = (Message) table.messages.elementAt(j);
        write(format("%s%s = %d;", prefix, message.name, j));
        writeln(format("%s[%s%s] = %s", var, prefix, message.name, message.value));
      }
      writeln();
    }
  }

  private static void generateEnumerators(Module module)
  {
    for (int i = 0; i < module.enumerators.size(); i++)
    {
      Enumerator enumerator = (Enumerator) module.enumerators.elementAt(i);
      String enumName = enumerator.name;
      writeln(format("%s = {}", enumName));
      int curr = 0;
      for (int j = 0; j < enumerator.elements.size(); j++)
      {
        String element = (String) enumerator.elements.elementAt(j);
        int n = element.indexOf('=');
        if (n != -1)
        {
          String value = element.substring(n + 1);
          curr = Integer.valueOf(value.trim());
          element = element.substring(0, n).trim();
        }
        write(format("%s%s = %d;", enumName.toLowerCase(), element, curr));
        writeln(format("%1$s[%2$s%3$s] = '%3$s'", enumName, enumName.toLowerCase(), element));
        curr += 1;
      }
      writeln();
    }
  }

  private static String getInit(Field field)
  {
    switch (field.type.typeof)
    {
      case Type.BOOLEAN:
      case Type.BYTE:
      case Type.SHORT:
      case Type.INT:
      case Type.LONG:
        return "0";
      case Type.CHAR:
      case Type.STRING:
        return "''";
      case Type.FLOAT:
      case Type.DOUBLE:
        return "0.0";
    }
    return "None";
  }

  private static void generateImplement(Module module, String output)
  {
    String fName = "%s%s_impl.py".formatted(output, module.name.toLowerCase());
    logln("Code: " + fName);
    try (var outData = new PrintWriter(new FileOutputStream(fName)))
    {
      writer = outData;
      indent_size = 4;
      writeln(format("'''%s idl2 implementation", module.name.toUpperCase()));
      writeln(1, "If this generated code needs to be changed, do not modify it here, make a copy to change and modify that to use.");
      writeln("'''");
      writeln("from dbapi_util import connect, DBError");
      generateImports(module, false);
      if (module.python.size() > 0)
        generatePython(module);
      for (int i = 0; i < methods.size(); i++)
      {
        Method method = methods.elementAt(i);
        Prototype prototype = method.prototype;
        writeln();
        generatePythonCode(prototype);
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

  private static void generatePython(Module module)
  {
    for (int i = 0; i < module.python.size(); i++)
    {
      String line = (String) module.python.elementAt(i);
      write(line);
    }
  }

  private static void generatePythonCode(Prototype prototype)
  {
    String parametersHint = "";
    String responseHint = "";
    String responseComment = "";
    String comma = "";
    String resp = "# ";
    boolean bracket = false;
    for (int i = 0; i < prototype.inputs.size(); i++)
    {
      Action action = (Action) prototype.inputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      parametersHint += comma + fieldInType(action, field);
      comma = ", ";
      resp = "# ";
    }
    comma = "";
    String ptr = "";
    int isize = 1;
    switch (prototype.type.typeof)
    {
      case Type.BYTE:
      case Type.SHORT:
      case Type.INT:
      case Type.LONG:
        comma = ", ";
        resp = ", ";
        ptr = " ->";
        isize = 0;
        responseHint = "int";
        responseComment += "# returnCode";
    }
    for (int i = 0; i < prototype.outputs.size(); i++)
    {
      ptr = " ->";
      bracket = i >= isize;
      Action action = (Action) prototype.outputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      responseHint += comma + fieldOutType(action, field);
      responseComment += resp + field.name;
      comma = ", ";
      resp = ", ";
    }
    String def;
    if (bracket)
      def = format("def %s(%s)%s (%s): %s", prototype.name, parametersHint, ptr, responseHint, responseComment);
    else
      def = format("def %s(%s)%s %s: %s", prototype.name, parametersHint, ptr, responseHint, responseComment);
    writeln(def);
    boolean done = false;
    int ind = 0;
    for (int i = 0; i < prototype.python.size(); i++)
    {
      String line = (String) prototype.python.elementAt(i);
      if (i == 0)
      {
        if (line.charAt(0) != ' ')
          ind = 1;
      }
      if (line.length() > 0)
      {
        write(ind, line);
        done = true;
      }
    }
    if (done == false)
      writeln(1, "pass");
  }

  private static void generatePythonJSON(Module module, String output)
  {
    String fName = "%s%s_json.py".formatted(output, module.name.toLowerCase());
    logln("Code: " + fName);
    try (var outData = new PrintWriter(new FileOutputStream(fName)))
    {
      writer = outData;
      indent_size = 4;
      writeln(format("'''%s json serializer/deserializer", module.name.toLowerCase()));
      writeln(1, "This code was generated, do not modify it, modify it at source and regenerate it.");
      writeln(1, format("We suspect that %s_impl.py may be hand coded to complete.", module.name.toLowerCase()));
      writeln("'''");
      writeln("from dbapi_util import connect, DBError, log_error");
      generateImports(module, false);
      writeln("import json");
      writeln(format("import %s_impl as impl", module.name.toLowerCase()));
      for (int i = 0; i < methods.size(); i++)
      {
        Method method = methods.elementAt(i);
        Prototype prototype = method.prototype;
        writeln();
        generatePythonDefs(prototype);
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

  private static void generateImports(Module module, boolean dbOnly)
  {
    for (int i = 0; i < schemas.size(); i++)
    {
      Schema schema = schemas.elementAt(i);
      if (schema.type == Schema.DB_STRUCT)
      {
        int n = schema.structure.header.length() - 3;
        writeln(format("from %sDBApi import %s", schema.structure.name.substring(2,n), schema.name));
      }
      else if (dbOnly == false)
        writeln(format("from %s import %s", module.name.toLowerCase(), schema.name));
    }
  }

  private static void generatePythonDefs(Prototype prototype)
  {
    String parameters = "", parametersHint = "";
    String response = "", responseHint = "", responseComment = "";
    String comma = "";
    String resp = "# ";
    boolean bracket = false;
    boolean hasResult = false;
    boolean hasInput = prototype.inputs.size() > 0;
    boolean hasOutput = prototype.outputs.size() > 0;
    for (int i = 0; i < prototype.inputs.size(); i++)
    {
      Action action = (Action) prototype.inputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      parameters += comma + field.name;
      parametersHint += comma + fieldInType(action, field);
      comma = ", ";
      resp = "# ";
    }
    comma = "";
    String ptr = "";
    int isize = 1;
    switch (prototype.type.typeof)
    {
      case Type.BYTE:
      case Type.SHORT:
      case Type.INT:
      case Type.LONG:
        comma = ", ";
        resp = ", ";
        ptr = " ->";
        isize = 0;
        response = "returnCode";
        responseHint = "int";
        responseComment += "# returnCode";
        hasResult = true;
    }
    for (int i = 0; i < prototype.outputs.size(); i++)
    {
      ptr = " ->";
      bracket = i >= isize;
      Action action = (Action) prototype.outputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      response += comma + field.name;
      responseHint += comma + fieldOutType(action, field);
      responseComment += resp + field.name;
      comma = ", ";
      resp = ", ";
    }
    String inputStr = "";
    String outputStr = ":";
    if (hasInput)
      inputStr = "input: str";
    if (hasOutput || hasResult)
      outputStr = " -> str: # output";
    writeln(format("def %s(%s)%s", prototype.name, inputStr, outputStr));
    writeln(1, "'''");
    String def;
    if (bracket)
      def = format("def %s(%s)%s (%s): %s", prototype.name, parametersHint, ptr, responseHint, responseComment);
    else
      def = format("def %s(%s)%s %s: %s", prototype.name, parametersHint, ptr, responseHint, responseComment);
    writeln(1, def);
    writeln(1, "'''");
    writeln(1, "output = ''");
    writeln(1, "try:");
    genDefBody(prototype, parameters, response, hasResult);
    writeln(1, "except DBError as db:");
    writeln(2, "error = 'DBError: value:{0} rc:{1} ociErr:{2}'.format(db.value, db.rc, db.ociErr)");
    writeln(2, "log_error(error)");
    writeln(2, "output = 'ERR400:%s' % (error)");
    writeln(1, "except BaseException as be:");
    writeln(2, "error = 'BaseException:{0}'.format(be.args)");
    writeln(2, "log_error(error)");
    writeln(2, "output = 'ERR400:%s' % (error)");
    writeln(1, "except:");
    writeln(2, "error = 'Except:{0}'.format(sys.exc_info()[0])");
    writeln(2, "log_error(error)");
    writeln(2, "output = 'ERR400:%s' % (error)");
    writeln(1, "return output");
  }

  private static void genDefBody(Prototype prototype, String parameters, String response, boolean hasResult)
  {
    int ind = 2;
    if (prototype.inputs.size() > 0)
    {
      writeln(ind, "request = json.loads(input)");
      writeln(ind, format("if impl.%s != None:", prototype.name));
      for (int i = 0; i < prototype.inputs.size(); i++)
      {
        Action action = (Action) prototype.inputs.elementAt(i);
        Field field = action.getParameter(prototype);
        if (field == null)
          continue;
        getInField(action, field);
      }
      if (hasResult == false && prototype.outputs.size() == 0)
      {
        writeln(ind + 1, format("impl.%s(%s)", prototype.name, parameters));
        return;
      }
      writeln(ind + 1, format("%s = impl.%s(%s)", response, prototype.name, parameters));
    } else
    {
      ind = 1;
      writeln(ind + 1, format("%s = impl.%s(%s)", response, prototype.name, parameters));
    }
    writeln(ind + 1, "response = {}");
    if (hasResult == true)
      writeln(ind + 1, "response['returnCode'] = returnCode");
    for (int i = 0; i < prototype.outputs.size(); i++)
    {
      Action action = (Action) prototype.outputs.elementAt(i);
      Field field = action.getParameter(prototype);
      if (field == null)
        continue;
      setOutField(action, field, ind);
    }
    writeln(ind + 1, "output = json.dumps(response)");
  }

  private static void setOutField(Action action, Field field, int ind)
  {
    Operation op = action.sizeOperation();
    if (field.type.typeof == Type.USERTYPE)
    {
    }
    switch (field.type.typeof)
    {
      case Type.BOOLEAN:
      case Type.BYTE:
      case Type.CHAR:
      case Type.SHORT:
      case Type.INT:
      case Type.LONG:
      case Type.FLOAT:
      case Type.DOUBLE:
      case Type.STRING:
        writeln(ind + 1, format("response['%1$s'] = %1$s", field.name));
        break;
      case Type.USERTYPE:
        if (op == null)
        {
          writeln(ind + 1, format("_entry = {}"));
          writeln(ind + 1, format("for _field in %1$s._fields():", field.name));
          writeln(ind + 2, format("_entry[_field] = getattr(%1$s, _field)", field.name));
          writeln(ind + 1, format("response['%1$s'] = _entry", field.name));
        } else
        {
          writeln(ind + 1, format("%1$s_items = []", field.name));
          writeln(ind + 1, format("for _item in %1$s:", field.name));
          writeln(ind + 2, format("_entry = {}"));
          writeln(ind + 2, format("for _field in _item._fields():"));
          writeln(ind + 3, format("_entry[_field] = getattr(_item, _field)"));
          writeln(ind + 2, format("%1$s_items.append(_entry)", field.name));
          writeln(ind + 2, format("response['%1$s'] = %1$s_items;", field.name));
        }
        break;
    }
  }

  private static void getInField(Action action, Field field)
  {
    Operation op = action.sizeOperation();
    String usertype = "<not_here>";
    if (field.type.typeof == Type.USERTYPE)
      usertype = getUserTypeName(field.type.name);
    switch (field.type.typeof)
    {
      case Type.BOOLEAN:
      case Type.BYTE:
      case Type.CHAR:
      case Type.SHORT:
      case Type.INT:
      case Type.LONG:
      case Type.FLOAT:
      case Type.DOUBLE:
      case Type.STRING:
        writeln(2 + 1, format("%1$s = request['%1$s']", field.name));
        break;
      case Type.USERTYPE:
        if (op == null)
        {
          writeln(2 + 1, format("%1$s_vars = request['%1$s']", field.name));
          writeln(2 + 1, format("%1$s = %2$s(%3$s)", field.name, usertype, typeArg(field)));
          writeln(2 + 1, format("for _field in %1$s._fields():", field.name));
          writeln(3 + 1, format("setattr(%1$s, _field, %1$s_vars[_field])", field.name));
        } else
        {
          writeln(2 + 1, format("%1$s_items = request['%1$s']", field.name));
          writeln(2 + 1, format("%1$s = []", field.name));
          writeln(2 + 1, format("for i in range(%1$s):", op.name));
          writeln(3 + 1, format("%1$s_vars = %1$s_items[i]", field.name));
          writeln(3 + 1, format("_rec = %s(%s)", usertype, typeArg(field)));
          writeln(3 + 1, "for _field in _rec._fields():");
          writeln(4 + 1, format("setattr(_rec, _field, %1$s_vars[_field])", field.name));
          writeln(3 + 1, format("%1$s.append(_rec)", field.name));
        }
        break;
    }
  }

  private static String typeArg(Field field)
  {
    for (int i = 0; i < schemas.size(); i++)
    {
      Schema schema = schemas.elementAt(i);
      if (schema.name.compareTo(field.type.name) == 0)
      {
        if (schema.type == Schema.DB_STRUCT)
          return "connect";
        break;
      }
    }
    return "";
  }

  private static String getUserTypeName(String name)
  {
    for (int i = 0; i < schemas.size(); i++)
    {
      Schema schema = schemas.elementAt(i);
      if (schema.name.compareTo(name) == 0)
      {
        if (schema.type == Schema.DB_STRUCT)
          return "D" + name.substring(1);
        if (schema.type == Schema.IDL_STRUCT)
          return "t" + name.substring(1);
        break;
      }
    }
    return name;
  }

  private static String fieldInType(Action action, Field field)
  {
    Operation op = action.sizeOperation();
    String fmt = "%s: %s";
    if (op != null && field.type.typeof == Type.USERTYPE)
      fmt = "%s: [%s]";
    switch (field.type.typeof)
    {
      case Type.BYTE:
      case Type.SHORT:
      case Type.INT:
        return format(fmt, field.name, "int");
      case Type.LONG:
        return format(fmt, field.name, "long");
      case Type.FLOAT:
      case Type.DOUBLE:
        return format(fmt, field.name, "float");
      case Type.STRING:
        return format(fmt, field.name, "str");
      case Type.USERTYPE:
        return format(fmt, field.name, getUserTypeName(field.type.name));
      default:
        return format(fmt, field.name, "str");
    }
  }

  private static String fieldOutType(Action action, Field field)
  {
    Operation op = action.sizeOperation();
    String fmt = "%s";
    if (op != null && field.type.typeof == Type.USERTYPE)
      fmt = "[%s]";
    switch (field.type.typeof)
    {
      case Type.BYTE:
      case Type.SHORT:
      case Type.INT:
        return format(fmt, "int");
      case Type.LONG:
        return format(fmt, "long");
      case Type.FLOAT:
      case Type.DOUBLE:
        return format(fmt, "float");
      case Type.STRING:
        return format(fmt, "str");
      case Type.USERTYPE:
        return format(fmt, getUserTypeName(field.type.name));
      default:
        return format(fmt, "str");
    }
  }
}

class Schema
{
  static final byte DB_STRUCT = 0, IDL_STRUCT = 1;
  String name;
  String header;
  byte type;
  Structure structure;
  boolean used;

  Schema(String name, String header, byte type, Structure structure)
  {
    this.name = name;
    this.header = header;
    this.type = type;
    this.structure = structure;
    this.used = false;
  }
}

class Method
{
  Prototype prototype;

  Method(Prototype prototype)
  {
    this.prototype = prototype;
  }
}
