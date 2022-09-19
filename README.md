## Mesh Smoothing
### Motivation
•Filter out high frequency noise

•Input : noisy mesh (scanned or other)

•Output : smooth mesh

•How : filter out high frequency noise

## Filtering on a Mesh
•Laplacian smoothing
Repeat for m iterations (for non boundary points):

𝒑𝒊←𝒑𝒊+𝝀𝑳𝒑𝒊 

𝟎<𝝀<𝟏

Closed curved converges to single point


