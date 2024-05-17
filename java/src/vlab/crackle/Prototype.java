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

import java.io.Serializable;
import java.util.Vector;

/**
 *
 */
public class Prototype implements Serializable
{
  public static final byte
          RPCCALL = 0, PUBLIC = 1, PRIVATE = 2, PROTECTED = 3;
  private static final long serialVersionUID = 1L;
  public String name;
  public String message;
  public Type type;
  public Vector parameters;
  public Vector inputs;
  public Vector outputs;
  public Vector categories; // used for VB only
  public byte codeType;
  public Vector code;
  public Vector python;
  public int codeLine;
  public int start;
  public OpenApi openApi;

  public Prototype()
  {
    name = "";
    message = "";
    type = new Type();
    parameters = new Vector();
    inputs = new Vector();
    outputs = new Vector();
    categories = new Vector();
    codeType = RPCCALL;
    code = new Vector();
    python = new Vector();
    codeLine = 0;
    start = 0;
    openApi = null;
  }

  public int hashCode(String data)
  {
    return Module.hashCode(data);
  }

  public void addParameter(Field field)
  {
    parameters.addElement(field);
  }

  public void addInput(Action action)
  {
    inputs.addElement(action);
    Field field = getParameter(action.name);
    if (field != null)
    {
      field.input = action;
      field.isInput = true;
      field.hasSize = action.hasSize();
    }
  }

  public void addOutput(Action action)
  {
    outputs.addElement(action);
    Field field = getParameter(action.name);
    if (field != null)
    {
      field.output = action;
      field.isOutput = true;
      field.hasSize = action.hasSize();
    }
  }

  public Field getParameter(String name)
  {
    for (int i = 0; i < parameters.size(); i++)
    {
      Field parameter = (Field) parameters.elementAt(i);
      if (parameter.name.compareTo(name) == 0)
        return parameter;
    }
    return null;
  }

  public Action getInputAction(String name)
  {
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
        return action;
    }
    return null;
  }

  public Action getOutputAction(String name)
  {
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
        return action;
    }
    return null;
  }

  public boolean hasSize(String name)
  {
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return true;
      }
    }
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return true;
      }
    }
    return false;
  }

  public boolean hasOutputSize(String name)
  {
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return true;
      }
    }
    return false;
  }

  public boolean hasInputSize(String name)
  {
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return true;
      }
    }
    return false;
  }

  public String getInputSizeName(String name)
  {
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return action.getSizeName();
      }
    }
    return "";
  }

  public String getSizeName(String name)
  {
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return action.getSizeName();
      }
    }
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return action.getSizeName();
      }
    }
    return "";
  }

  public String getOutputSizeName(String name)
  {
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      if (action.name.compareTo(name) == 0)
      {
        if (action.hasSize())
          return action.getSizeName();
      }
    }
    return "";
  }

  public long signature(boolean longAsInt)
  {
    long result = 0;
    result += hashCode(name);
    result += hashCode(message);
    for (int i = 0; i < parameters.size(); i++)
    {
      Field field = (Field) parameters.elementAt(i);
      int typeof = field.type.typeof;
      if (longAsInt == true && typeof == Type.INT)
        typeof = Type.LONG;
      result += (hashCode(field.name) * (typeof + 1));
      result = result % 7654321;
    }
    for (int i = 0; i < inputs.size(); i++)
    {
      Action action = (Action) inputs.elementAt(i);
      result += hashCode(action.name);
      result = result % 9876543;
    }
    for (int i = 0; i < outputs.size(); i++)
    {
      Action action = (Action) outputs.elementAt(i);
      result += hashCode(action.name);
      result = result % 5432109;
    }
    return result;
  }

  public boolean needsSwap()
  {
    return (type.typeof == Type.SHORT
            || type.typeof == Type.INT
            || type.typeof == Type.LONG
            || type.typeof == Type.FLOAT
            || type.typeof == Type.DOUBLE);
  }

  public boolean isRpcCall()
  {
    return codeType == RPCCALL;
  }

  public boolean isExtendedRpcCall()
  {
    if (codeType != PUBLIC)
      return false;
    return inputs.size() > 0 || outputs.size() > 0;
  }

  public boolean isPythonImpl()
  {
    if (python.size() > 0)
      return true;
    for (int i = 0; i < code.size(); i++)
    {
      String codeLine = (String) code.elementAt(i);
      if (codeLine.indexOf("PYTHON:") >= 0 && i == 0)
        return true;
    }
    return false;
  }

  public String toString()
  {
    return name;
  }
}
