#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
namespace fs = std::__fs::filesystem;

#include <pxr/base/tf/token.h>
#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usdGeom/scope.h>


// add names of prims that can never be the "default prim". 
// Typically a DCC or pipeline will add these prims for meta data purposes
const std::vector<std::string> BLACKLIST_PRIM = {"HoudiniLayerInfo", "MetadataInfo"};


// TODO: explore private and public functions. 

void raiseError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}


void printUsage() {
    raiseError("Usage: ./scope_reparent_run <usdFilePath> [scopeName]");
}

bool extractCommandLineArguments(int argc, char* argv[], std::string& usdFilePath, std::string& scopeName) {
    if (argc < 2 || argc > 3) {
        printUsage();
        return false;
    }

    usdFilePath = argv[1];

    // did the user enter a scope name?
    if (argc == 3) {
        // Yes!
        scopeName = argv[2];
    } else {
        // No :(
        std::cout << "Please enter the name of the scope prim you'd like to create: ";
        std::getline(std::cin, scopeName);
    }

    return true;
}


std::string deriveNewFilePath(const std::string& originalFilePath, const std::string& scopeName) {
    // Get the absolute path of the original file
    const fs::path absolutePath = fs::absolute(originalFilePath);

    // Get the directory and base filename of the absolute file path
    const std::string directory = absolutePath.parent_path().string();
    const std::string baseName = scopeName + "." + absolutePath.extension().string().substr(1);

    // Construct the result file path by appending the scope name and original extension to the directory path
    std::string resultFilePath = directory + "/" + baseName;
    return resultFilePath;
}

std::string getFileWithPrefix(const std::string& filePath) {
    fs::path path(filePath);
    std::string filename = path.stem().string();
    std::string extension = path.extension().string();
    return "./" + filename + extension;
}

// This function creates a new USD file with a given scope name and original file path.
// It returns true if successful, false otherwise.
bool createNewUsdFile(const std::string& originalFilePath, const std::string& scopeName) {
    
    // Derive the new file path from the original file path and scope name.
    std::string newFilePath = deriveNewFilePath(originalFilePath, scopeName);

    // Create a new USD stage using the derived file path.
    pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(newFilePath);

    // Check if the stage creation was successful.
    if (!stage) {
        // If not, print an error message and return false.
        raiseError("Error: Failed to create new USD stage.");
        return false;
    }

    // Define a new UsdGeomScope prim using the given scope name.
    pxr::TfToken scopeToken("Scope");
    pxr::UsdGeomScope scopePrim = pxr::UsdGeomScope::Define(stage, pxr::SdfPath("/" + scopeName));
    pxr::UsdModelAPI(scopePrim).SetKind(pxr::KindTokens->group);

    // Check if the scope prim creation was successful.
    if (!scopePrim) {
        // If not, print an error message and return false.
        raiseError("Error: Failed to create Scope prim.");
        return false;
    }

    // Add a reference to the original file path to the new scope prim.
    scopePrim.GetPrim().GetReferences().AddReference(getFileWithPrefix(originalFilePath));

    // Save the USD stage to the new file path.
    stage->Save();

    // Return true to indicate successful creation of the new USD file.
    return true;
}

bool hasDefaultPrim(const pxr::UsdStageRefPtr& stage, const std::string& filePath) {

    // Check if the stage has a default prim set
    pxr::UsdPrim defaultPrim = stage->GetDefaultPrim();
    if (!defaultPrim) {
        raiseError("Error: USD stage from file " + filePath + " has no default prim set");
        return false;
    }

    return true;
}


void printPrimList(const std::vector<pxr::UsdPrim>& prims) {
    std::cout << "Choose a default prim:" << std::endl;
    for (size_t i = 0; i < prims.size(); ++i) {
        std::cout << i << ": " << prims[i].GetName() << std::endl;
    }
}

int getUserPrimSelection(int numPrims) {
    int selection = -1;

    while (selection < 0 || selection >= numPrims) {
        std::cout << "Enter the number of the default prim: ";
        std::cin >> selection;

        if (std::cin.fail()) {
            // Clear the error flag and discard any remaining input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            selection = -1;
            continue;
        }

        if (selection < 0 || selection >= numPrims) {
            std::cout << "Invalid selection. Please enter a number between 0 and " << numPrims - 1 << "." << std::endl;
        }
    }

    return selection;
}


std::vector<pxr::UsdPrim> getChildPrims(const pxr::UsdPrim& parentPrim) {
    // Get the child prims of the parent prim, removing any blacklisted prims
    pxr::UsdPrimSiblingRange childPrimRange = parentPrim.GetChildren();
    std::vector<pxr::UsdPrim> childPrims(childPrimRange.begin(), childPrimRange.end());
    for (auto it = childPrims.begin(); it != childPrims.end();) {
        if (std::find(BLACKLIST_PRIM.begin(), BLACKLIST_PRIM.end(), it->GetName()) != BLACKLIST_PRIM.end()) {
            it = childPrims.erase(it);
        } else {
            ++it;
        }
    }
    return childPrims;
}

bool assignDefaultPrim(const pxr::UsdStageRefPtr& stage, const std::string& filePath) {

    pxr::UsdPrim parentPrim = stage->GetPseudoRoot();
    if (!parentPrim) {
        raiseError("USD stage from file " + filePath + " has no root prim");
        return false;
    }

    std::vector<pxr::UsdPrim> childPrims = getChildPrims(parentPrim);

    if (childPrims.size() == 1) {
        stage->SetDefaultPrim(childPrims[0]);
        stage->Save();
        return stage->GetDefaultPrim() != pxr::UsdPrim();
    }

    if (childPrims.empty()) {
        raiseError("USD stage from file " + filePath + " has no valid child prims");
        return false;
    }

    // asking the user to set the default prims.
    printPrimList(childPrims);
    int selection = getUserPrimSelection(childPrims.size());
    if (selection < 0 || selection >= childPrims.size()) {
        raiseError("Invalid selection");
        return false;
    }

    stage->SetDefaultPrim(childPrims[selection]);
    stage->Save();
    return stage->GetDefaultPrim() != pxr::UsdPrim();
}



int main(int argc, char* argv[]) {

    std::string usdFilePath, scopeName;
    
    if (!extractCommandLineArguments(argc, argv, usdFilePath, scopeName)) {
        return EXIT_FAILURE;
    }
    pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(usdFilePath);
    if (!stage) {
        raiseError("Error opening USD stage from file " + usdFilePath);
        return EXIT_FAILURE;
    }

    // Validate the stage has a default prim, otherwise this sort of reference will not work.
    if (!hasDefaultPrim(stage, usdFilePath) && !assignDefaultPrim(stage, usdFilePath)) {
        return 1;
    }

    if (!createNewUsdFile(usdFilePath, scopeName)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
