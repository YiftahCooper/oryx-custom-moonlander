# Custom QMK Build for ZSA Moonlander (Oryx Sync)

This repository allows you to sync your Oryx layout with custom QMK code.

## How to use

1.  **Run the Workflow**: Go to the [Actions tab](https://github.com/YiftahCooper/oryx-custom-moonlander/actions).
2.  Select **Fetch and build layout**.
3.  Click **Run workflow**.
    *   **Layout ID**: `3aMQz` (Default)
    *   **Geometry**: `moonlander/revb` (Default)
4.  Wait for the build to finish.
5.  Download the firmware artifact.

## Customization

After the first run, a folder named `3aMQz` will appear in the root of the repository (in the `main` branch).
You can edit `keymap.c`, `rules.mk`, and `config.h` inside that folder to add custom QMK features.

**To avoid merge conflicts:**
*   Add your custom code in blocks separated by blank lines from the Oryx-generated code.
*   Do not delete the Oryx-generated comments if possible.

## Updates

To update your layout from Oryx:
1.  Make changes in Oryx.
2.  Compile in Oryx (to save the version).
3.  Run the **Fetch and build layout** workflow again here.
    *   This will fetch the new Oryx version to the `oryx` branch.
    *   It will merge the changes into `main`.
    *   It will rebuild the firmware.

If merge conflicts occur, you will need to resolve them locally in the `main` branch and push the fix.
