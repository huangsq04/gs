// Fill out your copyright notice in the Description page of Project Settings.


#include "HexTerrainActor.h"

#pragma optimize("", off)
// Sets default values
AHexTerrainActor::AHexTerrainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->Mobility = EComponentMobility::Static;
	VerticesBase = {
		{20784.609376,0.000000,0.000000},
	    {0.000000,12000.000000,0.000000},
		{0.000000,-12000.000000, 0.000000}
	};
	MeshMaterial = LoadObject<UMaterial>(nullptr, TEXT("/HexTerrain/LandMaterial"));
}

// Called when the game starts or when spawned
void AHexTerrainActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHexTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AHexTerrainActor::CreateProceduralMesh()
{
	TArray<FVector> Normals;
	TArray<FVector2D> UV1;
	TArray<FVector2D> UV2;
	TArray<FVector2D> UV3;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;
	DrawTerrainVertices.Empty();
	TerrainTriangles.Empty();
	HexTextureCoordinates.Empty();
	HexGrids.Empty();

	CreateVerticesRecursive(VerticesBase, { {1, 0}, {0, 0}, {0, 1} },0);

	ProceduralMesh->ClearMeshSection(0);

	ProceduralMesh->SetMaterial(0, MeshMaterial);

	ProceduralMesh->CreateMeshSection(0, DrawTerrainVertices, TerrainTriangles, Normals, HexTextureCoordinates,
		UV1, UV2, UV3, VertexColors, Tangents, true);
}
int AHexTerrainActor::AddTerrainPoint(const FVector &Point, const FVector2D &UV)
{
	uint32 *Index;
	uint32 Hash = GetTypeHash(Point);
	Index = VertexHash.Find(Hash);
	if (Index != nullptr)
	{
		return *Index;
	}
	else
	{
		int VerticeIndex = VertexHash.Num();
		VertexHash.Add(Hash, VerticeIndex);
		DrawTerrainVertices.Add(Point);
		HexTextureCoordinates.Add(UV);

		return  VerticeIndex;
	}

}
void AHexTerrainActor::CreateVerticesRecursive(
	const TArray<FVector> &Points,
	const TArray<FIntPoint> &HexPoints,
	int Depth
)
{
	const FVector &P1 = Points[0];
	const FVector &P2 = Points[1];
	const FVector &P3 = Points[2];

	FVector EdgeCenter[3] = { (P1 + P2) / 2, (P2 + P3) / 2, (P3 + P1) / 2 };

	if (Depth >= MaxDepth)
	{
		const TArray<FVector2D> UVPs = { {1,1}, {0,0}, {1,1} };
		TArray<FVector> PitchVertices;
		TArray<FVector2D> PitchVerticesUV;
		TArray<int32>   PitchTriangles;

		FVector d2 = P1 - P2;
		FVector d3 = P3 - P2;

		FVector2D t2 = UVPs[0] - UVPs[1];
		FVector2D t3 = UVPs[2] - UVPs[1];

		for (int i = 0; i < PatchResolution + 1; i++)
		{
			for (int j = 0; j < i + 1; j++)
			{
				FVector P(double(double(1.0 - double(i) / double(PatchResolution))),
					double(double(double(j) / double(PatchResolution))), 0);

				PitchVerticesUV.Add(UVPs[1] + (t2 * P.X + t3 * P.Y));

				P = P2 + (d2 * P.X + d3 * P.Y);
				PitchVertices.Add(P);

				if (i > 0 && j > 0)
				{
					PitchTriangles.Add(uint8(PitchVertices.Num() - 1));
					PitchTriangles.Add(uint8(PitchVertices.Num() - 2));
					PitchTriangles.Add(uint8(PitchVertices.Num() - 2 - i));

					if (j < i)
					{
						PitchTriangles.Add(uint8(PitchVertices.Num() - 2 - i));
						PitchTriangles.Add(uint8(PitchVertices.Num() - 1 - i));
						PitchTriangles.Add(uint8(PitchVertices.Num() - 1));
					}
				}
			}
		}

		uint32 hash = 0;
		hash = GetTypeHash(P2);
		int TriIdx = AddTerrainPoint(P2, UVPs[1]);
		if (!HexGrids.Contains(hash))
		{
			FHexGrid GridInfo;
			GridInfo.VerticeIndex = TriIdx;
			GridInfo.HexLocations = { HexPoints[1].X, HexPoints[1].Y, 0 };

			HexGrids.Add(hash, GridInfo);
		}

		for (int Idx : PitchTriangles)
		{
			int TriangleIdx = AddTerrainPoint(PitchVertices[Idx], PitchVerticesUV[Idx]);

			TerrainTriangles.Add(TriangleIdx);
		}

		return;
	}

	FVector P[6] = { P1, P2, P3, EdgeCenter[0], EdgeCenter[1], EdgeCenter[2] };
	FIntPoint H[6] = { HexPoints[0] * 2, HexPoints[1] * 2, HexPoints[2] * 2,
	(HexPoints[0] + HexPoints[1]), (HexPoints[1] + HexPoints[2]), (HexPoints[2] + HexPoints[0]) };

	int Idx[12] = { 3, 5, 0,   4, 5, 3,     3, 1, 4,   2, 5, 4 };
	bool Valid[4] = { 1, 1, 1, 1 };

	for (int i = 0; i < 4; i++)
	{
		if (Valid[i])
		{
			int i1 = Idx[3 * i + 0];
			int	i2 = Idx[3 * i + 1];
			int i3 = Idx[3 * i + 2];

			CreateVerticesRecursive({ P[i1], P[i2], P[i3] }, { H[i1], H[i2], H[i3] }, Depth + 1);
		}
	}
}
TArray<FHexGrid> AHexTerrainActor::GetHexPoint()
{
	TArray<FHexGrid> Out;
	for (auto &Item : HexGrids)
	{
		Out.Add(Item.Value);
	}

	return Out;
}
#pragma optimize("", on)
