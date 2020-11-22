{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 1,
			"revision" : 0,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 8.0, 47.0, 1545.0, 995.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 14.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 2,
		"gridsize" : [ 10.0, 10.0 ],
		"gridsnaponopen" : 2,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 7,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "custom2",
		"boxes" : [ 			{
				"box" : 				{
					"fontsize" : 16.0,
					"id" : "obj-19",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 511.0, 600.0, 33.0, 27.0 ],
					"text" : "init"
				}

			}
, 			{
				"box" : 				{
					"format" : 6,
					"id" : "obj-17",
					"maxclass" : "flonum",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "bang" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 830.0, 740.0, 50.0, 25.0 ]
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.94, 0.94, 0.94, 1.0 ],
					"id" : "obj-18",
					"markercolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"markers" : [ -60, -48, -36, -24, -12, -6, 0, 6 ],
					"markersused" : 8,
					"maxclass" : "levelmeter~",
					"needlecolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 830.0, 683.0, 96.0, 48.0 ],
					"release" : 22
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 570.0, 660.0, 66.0, 25.0 ],
					"text" : "loadbang"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 570.0, 700.0, 99.0, 25.0 ],
					"text" : "Threshold -0.1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 3,
					"outlettype" : [ "signal", "signal", "signal" ],
					"patching_rect" : [ 660.0, 740.0, 161.0, 25.0 ],
					"text" : "dplimit~ 2"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-11",
					"lastchannelcount" : 0,
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 660.0, 776.0, 90.0, 138.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "live.gain~",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -70.0,
							"parameter_longname" : "live.gain~",
							"parameter_mmax" : 6.0
						}

					}
,
					"varname" : "live.gain~"
				}

			}
, 			{
				"box" : 				{
					"clipheight" : 100.0,
					"data" : 					{
						"clips" : [ 							{
								"absolutepath" : "drumLoop.aif",
								"filename" : "drumLoop.aif",
								"filekind" : "audiofile",
								"selection" : [ 0.0, 1.0 ],
								"loop" : 1,
								"content_state" : 								{
									"mode" : [ "basic" ],
									"quality" : [ "basic" ],
									"originaltempo" : [ 120.0 ],
									"formant" : [ 1.0 ],
									"basictuning" : [ 440 ],
									"pitchcorrection" : [ 0 ],
									"followglobaltempo" : [ 0 ],
									"play" : [ 0 ],
									"slurtime" : [ 0.0 ],
									"speed" : [ 1.0 ],
									"timestretch" : [ 0 ],
									"formantcorrection" : [ 0 ],
									"pitchshift" : [ 1.0 ],
									"originallength" : [ 0.0, "ticks" ],
									"originallengthms" : [ 0.0 ]
								}

							}
 ]
					}
,
					"id" : "obj-5",
					"maxclass" : "playlist~",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
					"patching_rect" : [ 660.0, 494.0, 303.0, 100.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 570.0, 600.0, 82.0, 25.0 ],
					"text" : "r fil4params"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-7",
					"maxclass" : "preset",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "preset", "int", "preset", "int" ],
					"patching_rect" : [ 15.0, 550.0, 100.0, 40.0 ],
					"preset_data" : [ 						{
							"number" : 1,
							"data" : [ 5, "obj-27", "toggle", "int", 0, 5, "obj-29", "toggle", "int", 1, 5, "obj-35", "toggle", "int", 1, 5, "obj-45", "toggle", "int", 1, 5, "obj-40", "toggle", "int", 1, 5, "obj-52", "live.dial", "float", 25.0, 5, "obj-53", "live.dial", "float", 1.0, 5, "obj-54", "live.dial", "float", 50.0, 5, "obj-55", "live.dial", "float", 1.5, 5, "obj-56", "live.dial", "float", -12.0, 5, "obj-58", "live.dial", "float", 20000.0, 5, "obj-57", "live.dial", "float", 1.0, 5, "obj-61", "live.dial", "float", 12959.8828125, 5, "obj-60", "live.dial", "float", 0.806931734085083, 5, "obj-59", "live.dial", "float", -5.0, 5, "obj-62", "live.dial", "float", -1.5, 5, "obj-64", "live.dial", "float", 2.169291257858276, 5, "obj-65", "live.dial", "float", 3.0, 5, "obj-66", "live.dial", "float", 5.0, 5, "obj-67", "live.dial", "float", 1.445540189743042, 5, "obj-68", "live.dial", "float", 0.990384936332703, 5, "obj-69", "live.dial", "float", 1.579373359680176, 5, "obj-70", "live.dial", "float", 1.5, 5, "obj-71", "live.dial", "float", 4054.923095703125, 5, "obj-72", "live.dial", "float", 2000.0, 5, "obj-73", "live.dial", "float", 1031.9573974609375, 5, "obj-74", "live.dial", "float", 144.531463623046875, 5, "obj-75", "live.dial", "float", 0.0 ]
						}
 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-76",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 15.0, 494.0, 84.0, 25.0 ],
					"text" : "s fil4params"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.847058823529412, 0.0, 0.996078431372549, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-75",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 20.0, 202.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[22]",
							"parameter_mmax" : 30.0
						}

					}
,
					"varname" : "live.dial[22]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.733333333333333, 0.011764705882353, 0.011764705882353, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-74",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 590.0, 330.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B1_Frq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 20.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[21]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 2000.0,
							"parameter_initial" : [ 160 ]
						}

					}
,
					"varname" : "live.dial[21]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.015686274509804, 0.737254901960784, 0.231372549019608, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-73",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 668.333333333333371, 330.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B2_Frq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 40.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[20]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 4000.0,
							"parameter_initial" : [ 397 ]
						}

					}
,
					"varname" : "live.dial[20]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.047058823529412, 0.0, 0.772549019607843, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-72",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 746.666666666666515, 330.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B3_Frq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 100.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[19]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 10000.0,
							"parameter_initial" : [ 1250 ]
						}

					}
,
					"varname" : "live.dial[19]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.737254901960784, 0.690196078431373, 0.035294117647059, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-71",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 825.0, 330.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B4_Frq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 200.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[46]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 20000.0,
							"parameter_initial" : [ 2500 ]
						}

					}
,
					"varname" : "live.dial[18]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.733333333333333, 0.011764705882353, 0.011764705882353, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-70",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 590.0, 160.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B1_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[47]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 6.0,
							"parameter_initial" : [ 0.6 ]
						}

					}
,
					"varname" : "live.dial[17]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.015686274509804, 0.737254901960784, 0.231372549019608, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-69",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 668.333333333333371, 160.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B2_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[48]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 6.0,
							"parameter_initial" : [ 0.6 ]
						}

					}
,
					"varname" : "live.dial[16]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.047058823529412, 0.0, 0.772549019607843, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-68",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 746.666666666666515, 160.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B3_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[49]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 6.0,
							"parameter_initial" : [ 0.6 ]
						}

					}
,
					"varname" : "live.dial[15]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.737254901960784, 0.690196078431373, 0.035294117647059, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-67",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 825.0, 160.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B4_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[50]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 6.0,
							"parameter_initial" : [ 0.6 ]
						}

					}
,
					"varname" : "live.dial[14]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.733333333333333, 0.011764705882353, 0.011764705882353, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-66",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 590.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B1_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[51]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 30.0,
							"parameter_initial" : [ 0 ]
						}

					}
,
					"varname" : "live.dial[13]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.015686274509804, 0.737254901960784, 0.231372549019608, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-65",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 668.333333333333371, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B2_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[52]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 30.0,
							"parameter_initial" : [ 0 ]
						}

					}
,
					"varname" : "live.dial[12]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.047058823529412, 0.0, 0.772549019607843, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-64",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 746.666666666666515, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B3_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[53]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 30.0,
							"parameter_initial" : [ 0 ]
						}

					}
,
					"varname" : "live.dial[11]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.737254901960784, 0.690196078431373, 0.035294117647059, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-62",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 825.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "B4_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[10]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 30.0,
							"parameter_initial" : [ 0 ]
						}

					}
,
					"varname" : "live.dial[10]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.462745098039216, 0.568627450980392, 0.498039215686275, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-59",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 1138.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HS_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[7]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 30.0,
							"parameter_initial" : [ 0 ]
						}

					}
,
					"varname" : "live.dial[7]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.462745098039216, 0.568627450980392, 0.498039215686275, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-60",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 1072.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HS_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[8]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 2.0,
							"parameter_initial" : [ 1 ]
						}

					}
,
					"varname" : "live.dial[8]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.462745098039216, 0.568627450980392, 0.498039215686275, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-61",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 1006.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HiShelf",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 1000.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[9]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 16000.0,
							"parameter_initial" : [ 8000 ]
						}

					}
,
					"varname" : "live.dial[9]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.36078431372549, 0.847058823529412, 0.764705882352941, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-57",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 1401.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HipQ",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[54]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 2.0,
							"parameter_initial" : [ 1.0 ]
						}

					}
,
					"varname" : "live.dial[5]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.36078431372549, 0.847058823529412, 0.764705882352941, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-58",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 1321.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "LopFreq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 600.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[6]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 20000.0,
							"parameter_initial" : [ 20000.0 ]
						}

					}
,
					"varname" : "live.dial[6]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.317647058823529, 0.364705882352941, 0.333333333333333, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-56",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 464.0, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "LS_GAIN",
							"parameter_type" : 0,
							"parameter_unitstyle" : 4,
							"parameter_mmin" : -30.0,
							"parameter_longname" : "live.dial[55]",
							"parameter_mmax" : 30.0
						}

					}
,
					"varname" : "live.dial[4]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.317647058823529, 0.364705882352941, 0.333333333333333, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-55",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 392.666666666666629, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "LS_Q",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[3]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 2.0,
							"parameter_initial" : [ 0.7 ]
						}

					}
,
					"varname" : "live.dial[3]"
				}

			}
, 			{
				"box" : 				{
					"activedialcolor" : [ 0.317647058823529, 0.364705882352941, 0.333333333333333, 1.0 ],
					"fontsize" : 14.0,
					"id" : "obj-54",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 321.333333333333258, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "LoShelf",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 20.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[2]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 400.0,
							"parameter_initial" : [ 80 ]
						}

					}
,
					"varname" : "live.dial[2]"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 14.0,
					"id" : "obj-53",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 162.5, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HipQ",
							"parameter_type" : 0,
							"parameter_unitstyle" : 1,
							"parameter_mmin" : 0.01,
							"parameter_exponent" : 1.5,
							"parameter_longname" : "live.dial[1]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 2.0,
							"parameter_initial" : [ 0.7 ]
						}

					}
,
					"varname" : "live.dial[1]"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 14.0,
					"id" : "obj-52",
					"maxclass" : "live.dial",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 91.25, 10.0, 80.0, 59.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_shortname" : "HipFreq",
							"parameter_type" : 0,
							"parameter_unitstyle" : 3,
							"parameter_mmin" : 10.0,
							"parameter_exponent" : 2.5,
							"parameter_longname" : "live.dial[56]",
							"parameter_initial_enable" : 1,
							"parameter_mmax" : 1000.0,
							"parameter_initial" : [ 20 ]
						}

					}
,
					"varname" : "live.dial"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-39",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 940.0, 76.5, 217.0, 25.0 ],
					"text" : "pak 0. 8000. 1. 0."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-40",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 940.0, 10.0, 40.0, 40.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-41",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 940.0, 106.5, 127.0, 25.0 ],
					"text" : "hishelf $1 $2 $3 $4"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-44",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 250.0, 74.0, 233.0, 25.0 ],
					"text" : "pak 0. 80. 0.7 0."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-45",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 250.0, 10.0, 40.0, 40.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-46",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 250.0, 104.0, 127.0, 25.0 ],
					"text" : "loshelf $1 $2 $3 $4"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-33",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1240.0, 76.5, 181.0, 25.0 ],
					"text" : "pak 0. 20000. 1."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-35",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 1240.0, 10.0, 40.0, 40.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-36",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 1240.0, 106.5, 107.0, 25.0 ],
					"text" : "lopass $1 $2 $3"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-30",
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 20.0, 74.0, 161.5, 25.0 ],
					"text" : "pak 0. 20. 0.7"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-29",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 20.0, 10.0, 40.0, 40.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-27",
					"maxclass" : "toggle",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 20.0, 334.0, 35.0, 35.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-21",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 232.0, 254.0, 25.0 ],
					"text" : "pak 0.6 0.6 0.6 0.6"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-22",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 267.0, 124.0, 25.0 ],
					"text" : "bands $1 $2 $3 $4"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-15",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 400.0, 254.0, 25.0 ],
					"text" : "pak 160. 397. 1250. 2500."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-16",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 440.0, 117.0, 25.0 ],
					"text" : "freqs $1 $2 $3 $4"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 80.0, 254.0, 25.0 ],
					"text" : "pak 0. 0. 0. 0."
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-8",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 590.0, 120.0, 120.0, 25.0 ],
					"text" : "gains $1 $2 $3 $4"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-23",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 20.0, 272.0, 70.0, 25.0 ],
					"text" : "Output $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 20.0, 374.0, 74.0, 25.0 ],
					"text" : "Bypass $1"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-26",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 20.0, 104.0, 107.0, 25.0 ],
					"text" : "hipass $1 $2 $3"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 14.0,
					"id" : "obj-4",
					"maxclass" : "spectrumdraw~",
					"numinlets" : 4,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 760.0, 776.0, 641.0, 193.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-2",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 656.0, 924.0, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"patching_rect" : [ 660.0, 654.0, 90.0, 25.0 ],
					"text" : "fil4~ 2"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 1 ],
					"midpoints" : [ 740.5, 717.0, 811.5, 717.0 ],
					"order" : 0,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"order" : 1,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"midpoints" : [ 740.5, 701.0, 769.5, 701.0 ],
					"order" : 1,
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"midpoints" : [ 669.5, 687.0, 769.5, 687.0 ],
					"order" : 0,
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 1 ],
					"source" : [ "obj-11", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-11", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"midpoints" : [ 579.5, 735.0, 669.5, 735.0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"source" : [ "obj-15", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 599.5, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-17", 0 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 520.5, 639.0, 669.5, 639.0 ],
					"source" : [ "obj-19", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-22", 0 ],
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 599.5, 315.0, 110.0, 315.0, 110.0, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 29.5, 321.0, 7.0, 321.0, 7.0, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-23", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 29.5, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 29.5, 189.0, 7.0, 189.0, 7.0, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-26", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-24", 0 ],
					"source" : [ "obj-27", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 0 ],
					"source" : [ "obj-29", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 1 ],
					"source" : [ "obj-3", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-18", 0 ],
					"midpoints" : [ 811.5, 768.0, 825.0, 768.0, 825.0, 678.0, 839.5, 678.0 ],
					"source" : [ "obj-3", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-36", 0 ],
					"source" : [ "obj-33", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-33", 0 ],
					"source" : [ "obj-35", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 1249.5, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-36", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-41", 0 ],
					"source" : [ "obj-39", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 0 ],
					"source" : [ "obj-40", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 949.5, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-41", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-46", 0 ],
					"source" : [ "obj-44", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-44", 0 ],
					"source" : [ "obj-45", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 259.5, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-46", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 1 ],
					"source" : [ "obj-5", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 1 ],
					"source" : [ "obj-52", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-30", 2 ],
					"source" : [ "obj-53", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-44", 1 ],
					"source" : [ "obj-54", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-44", 2 ],
					"source" : [ "obj-55", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-44", 3 ],
					"source" : [ "obj-56", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-33", 2 ],
					"midpoints" : [ 1410.5, 72.5, 1411.5, 72.5 ],
					"source" : [ "obj-57", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-33", 1 ],
					"source" : [ "obj-58", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 3 ],
					"source" : [ "obj-59", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-8", 0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 2 ],
					"source" : [ "obj-60", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 1 ],
					"source" : [ "obj-61", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 3 ],
					"source" : [ "obj-62", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 2 ],
					"source" : [ "obj-64", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 1 ],
					"source" : [ "obj-65", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-66", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 3 ],
					"source" : [ "obj-67", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 2 ],
					"source" : [ "obj-68", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 1 ],
					"source" : [ "obj-69", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-21", 0 ],
					"source" : [ "obj-70", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 3 ],
					"source" : [ "obj-71", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 2 ],
					"source" : [ "obj-72", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 1 ],
					"source" : [ "obj-73", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-15", 0 ],
					"source" : [ "obj-74", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-23", 0 ],
					"source" : [ "obj-75", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-76", 0 ],
					"midpoints" : [ 599.5, 147.0, 111.0, 147.0, 111.0, 480.0, 24.5, 480.0 ],
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 579.5, 639.0, 669.5, 639.0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-64" : [ "live.dial[53]", "B3_GAIN", 0 ],
			"obj-66" : [ "live.dial[51]", "B1_GAIN", 0 ],
			"obj-56" : [ "live.dial[55]", "LS_GAIN", 0 ],
			"obj-68" : [ "live.dial[49]", "B3_Q", 0 ],
			"obj-62" : [ "live.dial[10]", "B4_GAIN", 0 ],
			"obj-70" : [ "live.dial[47]", "B1_Q", 0 ],
			"obj-59" : [ "live.dial[7]", "HS_GAIN", 0 ],
			"obj-55" : [ "live.dial[3]", "LS_Q", 0 ],
			"obj-60" : [ "live.dial[8]", "HS_Q", 0 ],
			"obj-54" : [ "live.dial[2]", "LoShelf", 0 ],
			"obj-61" : [ "live.dial[9]", "HiShelf", 0 ],
			"obj-53" : [ "live.dial[1]", "HipQ", 0 ],
			"obj-75" : [ "live.dial[22]", "GAIN", 0 ],
			"obj-65" : [ "live.dial[52]", "B2_GAIN", 0 ],
			"obj-74" : [ "live.dial[21]", "B1_Frq", 0 ],
			"obj-73" : [ "live.dial[20]", "B2_Frq", 0 ],
			"obj-72" : [ "live.dial[19]", "B3_Frq", 0 ],
			"obj-67" : [ "live.dial[50]", "B4_Q", 0 ],
			"obj-57" : [ "live.dial[54]", "HipQ", 0 ],
			"obj-69" : [ "live.dial[48]", "B2_Q", 0 ],
			"obj-11" : [ "live.gain~", "live.gain~", 0 ],
			"obj-52" : [ "live.dial[56]", "HipFreq", 0 ],
			"obj-71" : [ "live.dial[46]", "B4_Frq", 0 ],
			"obj-58" : [ "live.dial[6]", "LopFreq", 0 ],
			"parameterbanks" : 			{

			}

		}
,
		"dependency_cache" : [ 			{
				"name" : "drumLoop.aif",
				"bootpath" : "C74:/media/msp",
				"type" : "AIFF",
				"implicit" : 1
			}
, 			{
				"name" : "fil4~.mxe64",
				"type" : "mx64"
			}
, 			{
				"name" : "spectrumdraw~.mxe64",
				"type" : "mx64"
			}
, 			{
				"name" : "dplimit~.mxe64",
				"type" : "mx64"
			}
 ],
		"autosave" : 0,
		"bgcolor" : [ 0.976470588235294, 0.976470588235294, 0.976470588235294, 1.0 ]
	}

}
