
import java.util.*;
import java.util.Scanner;  
import java.io.*;
import java.util.Map;
import java.util.HashMap;


public class SICASM {

	static int[] location = new int[50];
	static int startAddress = 0;
	static int programLength = 0;
	static Map<String,Integer> SYMTAB;
	static Map<String, String> OPTAB;

	public static void main(String args[]){
	
		try{
			if(args.length != 1){
				print("Error: missing file name. Usage: java SICASM sample.asm");
				System.exit(1);
			
			}
			
			SYMTAB = new HashMap<String, Integer>();
			OPTAB = new HashMap<String, String>();

			OPTAB.put("ADDR","90");
			OPTAB.put("COMPR","A0");
			OPTAB.put("SUBR","94");
			OPTAB.put("ADD","18");
			OPTAB.put("SUB","1C");
			OPTAB.put("MUL","20");
			OPTAB.put("DIV","24");
			OPTAB.put("COMP","28");
			OPTAB.put("J","3C");
			OPTAB.put("JEQ","30");
			OPTAB.put("JGT","34");
			OPTAB.put("JLT","38");
			OPTAB.put("JSUB","48");
			OPTAB.put("JSUB","48");
			OPTAB.put("LDCH","50");
			OPTAB.put("RSUB","4C");
			OPTAB.put("TIXR","B8");
			OPTAB.put("TIX","2C");
			OPTAB.put("RD","D8");
			OPTAB.put("TD","E0");
			OPTAB.put("WD","DC");
			OPTAB.put("STCH","54");
			OPTAB.put("CLEAR","B4");
			OPTAB.put("LD", "");
			OPTAB.put("LDA","00");
			OPTAB.put("LDB","68");
			OPTAB.put("LDL","08");
			OPTAB.put("LDS","60");
			OPTAB.put("LDT","74");
			OPTAB.put("LDX","04");
			OPTAB.put("ST","");
			OPTAB.put("STA","0C");
			OPTAB.put("STB","78");
			OPTAB.put("STL","14");
			OPTAB.put("STS","7C");
			OPTAB.put("STT","84");
			OPTAB.put("STX","10");

		
		pass1(args[0]);
		pass2(args[0]);
	
		}
		catch(Exception e){
		
		}
	
	} //end main



	static void pass2(String filename){
	
		String[] array;
		String temp_str = "";
		String label = "";
		String instruction = "";
		String operand = "";
		String objectCode = "";
		String target = "";
		String base = "";
		String opCode = "";
		String line= "";
		char register = 0;
		int opLength;
		int PC = 0;
		int n = 0;
		int i = 0;
		int x = 0;
		int b = 0;
		int p = 0;
		int e = 0;
		int temp_int = 0;
		int t1 = 0; //to show test output
		int t2 = 0; //to show test output
		int t3 = 0; //to show test output

		//PASS 2

		int line_count = 0;
		String file = new String(filename);
		try{
			BufferedReader reader = new BufferedReader(new FileReader(file));
		
			while((line = reader.readLine()) != null)
			{ 

				objectCode = "";
				label = "";
				instruction = "";
				operand = "";
				opCode = "";
				opLength = 0;

				n = 1;
				i = 1;
				x = 0;
				b = 0;
				p = 0;
				e = 0;

				if(line.contains("+")){
					e = 1;
					line = line.replace("+", "");
				}
				if(line.contains("@")){
					n = 1;
					i = 0;
					line = line.replace("@", "");
				}
				if(line.contains("#")){
					i = 1;
					n = 0;
					line = line.replace("#", "");
				}
				if(line.trim().startsWith("LDCH")){
					x = 1;
					line = "         LDCH      BUFFER";
				}
				if(line.trim().startsWith("STCH")){
					line = "         STCH      BUFFER";
				
				}
				
				//separate into label, instruction, and operand
				array = line.trim().split("\\s+");
				
				if(array.length == (3)){
					instruction = array[1];
					operand = array[2];
				}
				else if(array.length == (2)){
					instruction = array[0];
					operand = array[1];
				}
				else{
					line = line + " n/a"; //dummy operand to avoid out of bound indexing errors
					array = line.trim().split("\\s+");
					instruction = array[0];
				}

				
				//Subsitute ST with equivalent SIC instructions
				if(instruction.equals("ST")){
					array = operand.split(",");
					instruction = "ST" + array[1].substring(1);
					operand = array[0];
					}
				}

				//Subsitute LD with equivalent SIC instructions
				if(instruction.equals("LD")){
					array = operand.split(",");
					instruction = "LD" + array[0].substring(1);
					operand = array[1];
					}
		}

				if(OPTAB.containsKey(instruction)){
					opCode = OPTAB.get(instruction); 
					opLength = Integer.parseInt(array[1]);
					if(opCode.substring(n.length-1).equals('R')) {
						opLength = 2;
					}
					else {
						if(e == 1) {
							opLength = 4;
						}
						else {
							opLength = 3;
						}
					}					

					if(opLength == 2){
							
						n = 0;
						i = 0;

						if(operand.contains(",")){
							array = operand.split(",");
							register = array[0].charAt(1);
							target = "" + getRegisterNumber(register);
							register = array[1].charAt(1);
							target = target + getRegisterNumber(register);
						}
						else{
							register = operand.charAt(1);
							target = "" + getRegisterNumber(register) + "0";
							}
						
					}

					else if(instruction.equals("RSUB")){
						// RSUB is a special case	
						target = "000";
					}
						
					else if(i == 1)
						
						if(isNumeric(operand)){
						
							//create object code for immediate with number operands
							if(e == 1){
								
								//target = 20 bit hexadecimal operand number
								target = String.format("%05X", Integer.parseInt(operand));

							}
							else{
								
								//target = 12 bit hexadecimal operand number
								target = String.format("%03X", Integer.parseInt(operand));

							}
						else {
							//get address of label (operand)
							temp_int = SYMTAB.get(operand);
							if(e == 1) {
								//target = 20 bit hexadecimal operand number
								target = String.format("%05X", temp_int);
							}
							else {
								//target = 12 bit hexadecimal operand number
								target = String.format("%03X", temp_int);
							}
						}

					}

					else if(e == 1){
						
							//target = 20 bit hexadecimal operand address (from SYMTAB lookup)
							temp_int = SYMTAB.get(operand);
							target = String.format("%05X", temp_int);
					}

					else{
						temp_int = SYMTAB.get(operand);
						t1 = temp_int;
						PC = location[line_count] + opLength;
						t2 = PC;
						temp_int = temp_int - PC;
						t3 = temp_int;

						if(temp_int >= -2048 && temp_int < 2048){
							p = 1;
							target = String.format("%03X", temp_int);
						}
						else{
							if(base.length() > 0){
								temp_int = SYMTAB.get(operand);
								temp_int = temp_int - SYMTAB.get(base);
								if(temp_int >= 0 || temp_int <= 4096){
									//print("BASE OFFSET: " + temp_int);
									b = 1;
									target = String.format("%03X", temp_int);
								}
								else{
									print("ERROR: out of range.");
									System.exit(1);
								}

							}
							else{
								print("ERROR: out of range and base not set.");
								System.exit(1);
							}
						}
					}
					
				} // end (if instruction contains op code)

					else if(instruction.equals("BASE")){
					base = operand;
				}
			
				else if(instruction.equals("NOBASE")){
					base = "";
				}
				
				if(OPTAB.containsKey(instruction)){
					temp_int = Integer.parseInt(opCode, 16);
					temp_int = temp_int + (2 * n) + (i);
					objectCode = String.format("%02X", temp_int);
					if(opLength != 2){
						temp_int = (8 * x) + (4 * b) + (2 * p) + e;
						temp_str= String.format("%1X", temp_int);
						objectCode = objectCode + temp_str;
					}
					if(target.startsWith("FFFF")){
						target = target.substring(target.length()-3);
					}
					
					//FOR TESTING SHOW OUTPUT
					System.out.format(":: %04d", location[line_count]);
					print(" " + line);
					print("target: " + t1);
					print("PC: " + t2);
					print("OFFSET: " + t3);
					objectCode = objectCode + target; 
					print(">>>" + objectCode + "<<<");
					print("");
				}
			
			} //end while

			reader.close();
		
		} // end try

		catch (Exception err){
			 System.out.println(err.getMessage()) ;
			 System.exit(1);
		}

	} // end pass2


	static void pass1(String filename){
		String[] array;		//temporary String array for splitting line
		String temp_str;	//temporary String variable
		String label = "";
		String instruction = "";
		String operand = "";
		int LOCCTR = 0;
		int line_num = 0;
		boolean isExtended = false;

		try{
			
			String line;
			BufferedReader reader = new BufferedReader(new FileReader(filename));
			
			//Read first line and get starting address of the program
			line = reader.readLine();
			array = line.trim().split("\\s+");
			startAddress = Integer.parseInt(array[2]);
			LOCCTR = startAddress;
			
			//Read remaining lines of program to get labels and label addresses
			while((line = reader.readLine()) != null)
			{ 

				array = line.trim().split("\\s+");
				
				if(array[0].equals("END")){
					programLength = LOCCTR - startAddress;
					break;
				}	

				if(array.length == 3){
					if(SYMTAB.containsKey(array[0])){
						System.out.println("ERROR: DUPLICATE KEY");
					}
					else{
						//put address of label in SYMTAB
						SYMTAB.put(array[0], LOCCTR);
					}
				}
				
				//split instruction to find instruction length
				if(array.length == (3)){
					instruction = array[1];
					operand = array[2];
				}
				else if(array.length == (2)){
					instruction = array[0];
					operand = array[1];
				}
				else{
					instruction = array[0];
				}
				
				if (line.contains("+")){
					isExtended = true;
					instruction = instruction.substring(1);
				}
				else{
					isExtended = false;
				}
				
				// OP
				if(OPTAB.containsKey(instruction)){
					if(instruction.substring(n.length-1).equals('R')) {
						LOCCTR += 2;
					}
					else {
						if(isExtended) {
							LOCCTR += 4;
						}
						else {
							LOCCTR += 3;
						}
					}
				}
				
				// WORD
				else if(instruction.equals("WORD")){
					//increment lcctr 3
					LOCCTR += 3;
				}
				
				// RESW
				else if(instruction.equals("RESW")){
					//increment lcctr (3 * operand)
					LOCCTR += Integer.parseInt(operand)*3;
			
				}
				
				//BYTE
				else if(instruction.equals("BYTE")){
					if (operand.startsWith("C"))
					{
						LOCCTR += 3;
					}else{
						LOCCTR += 1;
					}
				}
				
				//RESB
				else if(instruction.equals("RESB")){
					//increment location by (operand)
					LOCCTR += Integer.parseInt(operand);
				}
					
			} //end while
			
			reader.close();

		} //end try
		
		catch (Exception err){
			 System.out.println(err.getMessage()) ;
			 System.exit(1);
		}

	} //end pass1

	static void print(String arg){
		System.out.println(arg);
	}

	public static boolean isNumeric(String str){  
	    try{  
		    int d = Integer.parseInt(str);  
	    }  
	    catch(Exception e){  
		    return false;  
	    }  
	    return true;  
	}

	public static int getRegisterNumber(char register){
		
		switch(register){
			case 'A': return 0;
			case 'B': return 3;
			case 'S': return 4;
			case 'T': return 5;
			case 'X': return 1;
			default: return 127;
		}
		
	}

}



