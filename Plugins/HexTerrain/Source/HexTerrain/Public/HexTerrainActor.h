// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "HexTerrainActor.generated.h"

USTRUCT(BlueprintType)
struct HEXTERRAIN_API FHexGrid
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VerticeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector HexLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SidesVerticeIndex;

};

UCLASS()
class HEXTERRAIN_API AHexTerrainActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTerrainActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "HexTerrain")
	void CreateProceduralMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTerrain")
	int MaxDepth = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTerrain")
	int PatchResolution = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTerrain")
	TArray<FVector> VerticesBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTerrain")
	class UMaterialInterface* MeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTerrain")
	TArray<FVector> DrawTerrainVertices;


	UFUNCTION(BlueprintCallable, Category = "HexTerrain")
	TArray<FHexGrid>  GetHexPoint();

private:

	void CreateVerticesRecursive(const TArray<FVector> &Points, const TArray<FIntPoint> &HexPoints, int Depth);

	int AddTerrainPoint(const FVector &Point, const FVector2D &UV);

	class UProceduralMeshComponent* ProceduralMesh;

	TArray<int32> TerrainTriangles;

	TMap<uint32, uint32>  VertexHash;

	TArray<FVector2D> HexTextureCoordinates;

	TMap<uint32, FHexGrid> HexGrids;

};
