# Copyright 2015 University of Wyoming
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

#!/usr/bin/env python
import pandas as pd
import os

# This script will read the mesh geometry from this directory
# the files it will read are:
#
# input_directory_path/mesh.1.ele
# input_directory_path/mesh.1.node
input_directory_path = "/share/CI-WATER_Simulation_Data/WBNFLSnake_mesh"

# This script will write its output to this directory
# the files it will write are:
#
# output_directory_path/elements.wkt
output_directory_path = "/share/CI-WATER_Simulation_Data/WBNFLSnake_mesh"
#Triangle Node and Element files
ELEfilepath           = os.path.join(input_directory_path, 'mesh.1.ele')
NODEfilepath          = os.path.join(input_directory_path, 'mesh.1.node')
#WKT output file
output_element_wkt_file = os.path.join(output_directory_path, 'elements.wkt')


def addWKT(s):
    """
        Helper fucntion that takes an element series and appends information to its WKT column
    """
    #Get coordinates for vertex 1
    s['WKT'] += str(nodes.loc[s['V1']]['X']) + " " + str(nodes.loc[s['V1']]['Y'])
    #Get coordinates for vertex 2
    s['WKT'] += ", " + str(nodes.loc[s['V2']]['X']) + " " + str(nodes.loc[s['V2']]['Y'])
    #Get coordinates for vertex 3
    s['WKT'] += ", " + str(nodes.loc[s['V3']]['X']) + " " + str(nodes.loc[s['V3']]['Y'])
    #Complete polygon by adding coordinates for vertex 1
    s['WKT'] += ", " + str(nodes.loc[s['V1']]['X']) + " " + str(nodes.loc[s['V1']]['Y'])
    #Complete the WKT string
    s['WKT'] += "))"
    
    return s


#Read nodes and elements into pandas data frames
elements = pd.read_csv(ELEfilepath, sep=' ', skipinitialspace=True, comment='#', skiprows=1, names=['ID', 'V1', 'V2', 'V3', 'CatchmentNumber'], index_col=0, engine='c').dropna()
nodes = pd.read_csv(NODEfilepath, sep=' ', skipinitialspace=True, comment='#', skiprows=1, names=['ID', 'X', 'Y', 'Z'], index_col=0, engine='c').dropna()

#Create the WKT polygon prefix for each element
elements['WKT'] = 'POLYGON(('
#Complete the polygon information
elements = elements.apply(addWKT, axis=1)

elements['WKT'].to_csv(output_element_wkt_file, index=False, header=['Polygon'])
