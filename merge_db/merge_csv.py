import pandas as pd
import numpy as np

prologue_name = "proteins_family_public_"

filename_protein = prologue_name + "protein.csv"
filename_prot_loc = prologue_name + "prot_loc.csv"
filename_organism = prologue_name + "organism.csv"
filename_localisation = prologue_name + "localisation.csv"


fields_prot = ['id', 'accn']
fields_prot_loc = ['id_prot', 'id_localisation']
fields_loca = ['id']

prot = pd.read_csv(filename_protein, usecols=fields_prot, nrows=100)
lasts_prots = pd.read_csv(filename_protein, usecols=fields_prot)
lasts_prots = lasts_prots.tail(50)


prot_loc = pd.read_csv(filename_prot_loc, usecols=fields_prot_loc)
#loca = pd.read_csv(filename_localisation, usecols=fields_loca)


#df = prot_loc.merge(loca, left_on="id_localisation", right_on="id")
#df = df.drop(columns=['id', 'id_localisation'])


df = prot_loc.merge(prot, left_on="id_prot", right_on="id")
df2 = prot_loc.merge(lasts_prots, left_on="id_prot", right_on="id")
#df = df.drop(columns=['id', 'id_prot'])


df = df.groupby('accn', as_index=False).agg(lambda x: x.tolist())
df = df.drop(columns=['id', 'id_prot'])
df2 = df2.groupby('accn', as_index=False).agg(lambda x: x.tolist())
df2 = df2.drop(columns=['id', 'id_prot'])




"""
df = prot.merge(prot_loc, left_on="id", right_on="id_prot")
df = df.drop(columns=['id', 'id_prot'])


df = df.merge(loca, left_on="id_localisation", right_on="id")
df = df.drop(columns=['id', 'id_localisation'])
"""
df.to_csv("./output.csv", index=False, header=True)
df2.to_csv("./output_lasts.csv", index=False, header=True)

