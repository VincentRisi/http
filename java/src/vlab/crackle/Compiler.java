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

package vlab.crackle;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringReader;
import java.io.File;
import java.lang.reflect.Method;
import java.util.Vector;

public class Compiler
{
  /**
   * Reads input from stored repository
   */
  static PrintWriter outLog;
  static String preprocessFlags = "";
  static String rootDir = "";
  static String idlFileName = "";
  static String iiDirName = "";
  static String ibDirName = "";
  static String iiFilesList = "";
  static String ibFilesList = "";
  static boolean preprocess = false;
  static boolean combine = false;
  //static boolean useOldHash = false;
  static Vector messageNos;

  private static boolean mkPath(String pathname)
  {
    File path = new File(pathname);
    boolean result = path.exists();
    if (result == false)
      result = path.mkdirs();
    return result;
  }

  private static boolean mkFilesPath(String filename)
  {
    File check = new File(filename);
    if (check.getParent() == null)
      return true;
    boolean result = mkPath(check.getParent());
    if (result == false)
      outLog.println("Directory of " + filename + " did not make.");
    return result;
  }

  private static String checkRoot(String name)
  {
    if (rootDir.length() == 0)
      return name;
    if (name.contains("${root}"))
       return name.replace("${root}", rootDir);
    if (name.contains("{root}"))
       return name.replace("{root}", rootDir);
    return name;
  }

  private static String[] argsParse(String[] args) throws FileNotFoundException
  {
    boolean reported = false;
    int i = 0, no = args.length;
    while (i < no)
    {
      String arg = args[i];
      if (arg.charAt(0) != '-')
        break;
      if (arg.length() == 1)
      {
        outLog.println("A singular '-' is an invalid command line argument");
        i++;
        break;
      }
      if (arg.charAt(1) == '-')
        break;
      switch (arg.charAt(1))
      {
        case 'R':
          if (arg.length() > 2)
            rootDir = arg.substring(2);
          else if (i < no && args[i + 1].charAt(0) != '-')
          {
            i++;
            rootDir = args[i];
          } else
            rootDir = "";
          break;
        case 'b':
          if (arg.length() > 2)
            ibDirName = checkRoot(arg.substring(2));
          else if (i < no && args[i + 1].charAt(0) != '-')
          {
            i++;
            ibDirName = checkRoot(args[i]);
          } else
            ibDirName = "";
          combine = true;
          break;
        case 'i':
          if (arg.length() > 2)
            iiDirName = checkRoot(arg.substring(2));
          else if (i < no && args[i + 1].charAt(0) != '-')
          {
            i++;
            iiDirName = checkRoot(args[i]);
          } else
            iiDirName = "";
          combine = true;
          break;
        case 'B':
          if (arg.length() > 2)
            ibFilesList = checkRoot(arg.substring(2));
          else if (i < no && args[i + 1].charAt(0) != '-')
          {
            i++;
            ibFilesList = checkRoot(args[i]);
          } else
            ibFilesList = "";
          combine = true;
          break;
        case 'I':
          if (arg.length() > 2)
            iiFilesList = checkRoot(arg.substring(2));
          else if (i < no && args[i + 1].charAt(0) != '-')
          {
            i++;
            iiFilesList = checkRoot(args[i]);
          } else
            iiFilesList = "";
          combine = true;
          break;
        case 'l':
          String logName;
          if (arg.length() > 2)
            logName = checkRoot(arg.substring(2));
          else if (i < no)
          {
            i++;
            logName = checkRoot(args[i]);
          } else
            break;
          if (mkFilesPath(logName) == true)
          {
            outLog.println("Switch Logging to " + logName);
            OutputStream outFile = new FileOutputStream(logName);
            outLog.flush();
            outLog = new PrintWriter(outFile);
          }
          break;
        case 'f':
          if (arg.length() > 2)
            idlFileName = checkRoot(arg.substring(2));
          else if (i < no)
          {
            i++;
            idlFileName = checkRoot(args[i]);
          }
          mkFilesPath(idlFileName);
          break;
        case 'h':
          //useOldHash = true;
          Module.j2Hash = 0;
          break;
        case 's':
          //useStringHash = true;
          Module.j2Hash = 2;
          break;
        case 'p':
          if (arg.length() > 2)
            preprocessFlags = arg.substring(2);
          else if (i < no)
          {
            String check = args[i + 1];
            if (check.charAt(0) != '-')
            {
              i++;
              preprocessFlags = args[i];
            }
          }
          preprocess = true;
          break;
        default:
          outLog.println("'" + arg + "' is an invalid command line argument");
          if (reported == false)
          {
            reported = true;
            outLog.println("Valid command line arguments here are");
            outLog.println(" -b <dir>    Directory containing source with .ib extension for business logic");
            outLog.println(" -B <file>   File containing list of source for business logic");
            outLog.println(" -f <name>   Filename of .idl file to be combined");
            outLog.println(" -h          Use previous older version 1.1.8 hash");
            outLog.println(" -i <dir>    Directory containing source with .ii extension from db .si files");
            outLog.println(" -I <file>   File containing list of source from db .si files");
            outLog.println(" -l <name>   Change logging name");
            outLog.println(" -p <string> Preprocess flags");
            outLog.println(" -s          Use post older than version 1.1.8 string hash");
          }
          break;
      }
      i++;
    }
    if (i == 0)
      return args;
    String[] result = new String[no - i];
    System.arraycopy(args, i, result, 0, no - i);
    return result;
  }

  public static void main(String[] args)
  {
    outLog = new PrintWriter(System.out);
    outLog.println("Starting crackle compile");
    try
    {
      args = argsParse(args);
      Module module;
      if (combine == true)
      {
        outLog.println("Combining" + " (" + Module.j2Hash + ")");
        messageNos = new Vector();
        String fileName = checkRoot(args[0]);
        String code = combineFileReader(fileName);
        if (idlFileName.length() > 0)
        {
          FileWriter fileWriter = new FileWriter(idlFileName);
          PrintWriter writer = new PrintWriter(fileWriter);
          writer.println(code);
          writer.flush();
          writer.close();
          fileWriter.close();
        }
        StringReader reader = new StringReader(code);
        module = IDL.run(fileName, reader, outLog);
      } else if (preprocess == true)
      {
        outLog.println("Prepocessing" + " (" + Module.j2Hash + ")");
        messageNos = new Vector();
        String code = processFileReader(args[0]);
        StringReader reader = new StringReader(code);
        module = IDL.run(args[0], reader, outLog);
      } else
      {
        outLog.println("Compiling file " + args[0] + " (" + Module.j2Hash + ")");
        module = IDL.run(args[0], outLog);
        outLog.println("Compile completed of " + args[0]);
      }
      if (module == null)
      {
        outLog.println("Error: Compile aborted due to errors");
        outLog.flush();
        System.exit(1);
      }
      String output = "";
      for (int i = 1; i < args.length; i++)
      {
        if (args[i].equals("-o"))
        {
          if (i + 1 < args.length)
          {
            output = checkRoot(args[++i]);
            char term = '\\';
            if (output.indexOf('/') != -1)
              term = '/';
            char ch = output.charAt(output.length() - 1);
            if (ch != term)
              output = output + term;
            mkPath(output);
            outLog.println("Generating to " + output);
          }
          continue;
        } else if (args[i].equals("-l"))
        {
          if (i + 1 < args.length)
          {
            String log = checkRoot(args[++i]);
            mkFilesPath(log);
            OutputStream outFile = new FileOutputStream(log);
            outLog.flush();
            outLog = new PrintWriter(outFile);
          }
          continue;
        }
        outLog.println(args[i]);
        Class c = Class.forName("vlab.crackle." + args[i]);
        Class[] d = {module.getClass(), output.getClass(), outLog.getClass()};
        Method m = c.getMethod("generate", d);
        Object[] o = {module, output, outLog};
        m.invoke(null, o);
      }
      outLog.flush();
      System.exit(0);
    }
    catch (Throwable e)
    {
      outLog.println("Error: " + e + "[" + e.getMessage() + "]");
      e.printStackTrace();
      outLog.flush();
      System.exit(1);
    }
  }

  private static int uniqueNumber(Vector messageNos, int procNumber)
  {
    if (procNumber < 0)
      procNumber *= -1;
    incProcNumber:
    for (; ; procNumber++)
    {
      for (int i = 0; i < messageNos.size(); i++)
      {
        Integer n = (Integer) messageNos.elementAt(i);
        if (n == procNumber)
          continue incProcNumber;
      }
      messageNos.addElement(procNumber);
      break;
    }
    return procNumber;
  }

  /*
   * with a do preprocess flag in the form below - I use the colons as a
   * convention the :xxx: is tested to see if it is contained in the string
   * -p:drop:keep:
   *
   * //$ifnot :drop:
   *   code
   *   code
   * //$endif
   * ...
   * //$if :keep:
   *   code
   *   code
   * //$endif
   */
  static String processFileReader(String fileName)
  {
    StringBuilder builder = new StringBuilder();
    try
    {
      outLog.println(fileName + " to be read");
      outLog.flush();
      FileReader fileReader = new FileReader(fileName);
      BufferedReader bufferedReader = new BufferedReader(fileReader);
      try
      {
        boolean insertLine = true;
        while (bufferedReader.ready())
        {
          String line = checkRoot(bufferedReader.readLine());
          String w = line;
          if (w.indexOf("//$include") == 0)
          {
            String newFileName = w.substring(10).trim();
            builder.append(processFileReader(newFileName));
            continue;
          }
          if (w.indexOf("//$message") == 0)
          {
            builder.append(String.format("message:%d;", uniqueNumber(messageNos, w.substring(10).trim().hashCode())));
            continue;
          }
          if (w.indexOf("//$if") == 0)
          {
            boolean sense = true;
            w = w.substring(5).trim();
            if (w.indexOf("not") == 0)
            {
              sense = false;
              w = w.substring(3).trim();
            }
            if (preprocessFlags.indexOf(w) != -1)
              insertLine = sense;
            else
              insertLine = !sense;
            outLog.println((insertLine == true ? "keeping " : "dropping ") + w);
            continue;
          }
          if (w.indexOf("//$endif") == 0)
          {
            insertLine = true;
            continue;
          }
          if (insertLine == true)
          {
            builder.append(line);
            builder.append('\n');
          }
        }
        bufferedReader.close();
        fileReader.close();
      }
      catch (NullPointerException e2)
      {
      }
    }
    catch (FileNotFoundException e)
    {
      outLog.println("PreProcess FileNotFoundException");
      outLog.flush();
    }
    catch (IOException e)
    {
      outLog.println("PreProcess IOException");
      outLog.flush();
    }
    return builder.toString();
  }

  private static String combineFileReader(String fileName)
  {
    StringBuilder builder = new StringBuilder();
    builder.append(processFileReader(fileName));
    appendFromDir(builder, iiDirName, ".ii");
    appendFromList(builder, iiFilesList);
    appendFromDir(builder, ibDirName, ".ib");
    appendFromList(builder, ibFilesList);
    return builder.toString();
  }

  private static String fullName(String dirName, String fileName)
  {
    String dirsep = "/";
    if (dirName.length() > 2 && dirName.charAt(1) == ':')
      dirsep = "\\";
    String result = dirName + dirsep + fileName;
    return result;
  }

  private static void appendFromDir(StringBuilder builder, String dirName, String ext)
  {
    if (dirName.length() == 0)
      return;
    dirName = checkRoot(dirName);
    File dir = new File(dirName);
    String[] list = dir.list();
    if (list == null)
    {
      outLog.println(dirName + " does not appear to be a good file directory");
      outLog.flush();
    } else
    {
      for (int i = 0; i < list.length; i++)
      {
        String name = list[i];
        int n = name.length();
        if (n <= 3)
          continue;
        if (name.substring(n - 3).compareTo(ext) == 0)
        {
          String combName = fullName(dirName, name);
          builder.append(String.format("// -- %s", combName));
          builder.append('\n');
          builder.append(processFileReader(combName));
        }
      }
    }
  }

  private static void appendFromList(StringBuilder builder, String filesList)
  {
    try
    {
      try
      {
        FileReader fileReader = new FileReader(filesList);
        BufferedReader bufferedReader = new BufferedReader(fileReader);
        while (bufferedReader.ready())
        {
          String fileName = checkRoot(bufferedReader.readLine());
          builder.append(processFileReader(fileName));
        }
        bufferedReader.close();
        fileReader.close();
      }
      catch (FileNotFoundException e)
      {
        outLog.println("PreProcess FileNotFoundException");
        outLog.flush();
      }
    }
    catch (FileNotFoundException e)
    {
      outLog.println("PreProcess FileNotFoundException");
      outLog.flush();
    }
    catch (IOException e)
    {
      outLog.println("PreProcess IOException");
      outLog.flush();
    }
  }
}
